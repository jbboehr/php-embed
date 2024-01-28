{
  description = "php-embed";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-22.11";
    flake-utils = {
      url = "github:numtide/flake-utils";
    };
    gitignore = {
      url = "github:hercules-ci/gitignore.nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    pre-commit-hooks = {
      url = "github:cachix/pre-commit-hooks.nix/nixos-21.11";
      inputs.nixpkgs.follows = "nixpkgs";
      inputs.nixpkgs-stable.follows = "nixpkgs";
      inputs.flake-utils.follows = "flake-utils";
      inputs.gitignore.follows = "gitignore";
    };
    ext-ast = {
      url = "github:nikic/php-ast";
      flake = false;
    };
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    gitignore,
    pre-commit-hooks,
    ext-ast,
  } @ args:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = nixpkgs.legacyPackages.${system};

        src' = gitignore.lib.gitignoreSource ./.;

        src = pkgs.lib.cleanSourceWith {
          name = "php-embed-source";
          src = src';
          filter = gitignore.lib.gitignoreFilterWith {
            basePath = ./.;
            extraRules = ''
              .clang-format
              composer.json
              composer.lock
              .editorconfig
              .envrc
              .gitattributes
              .github
              .gitignore
              *.md
              *.nix
              flake.*
            '';
          };
        };

        makePhp = php:
          php.buildEnv {
            extraConfig = ''
              include_path = .:${ext-ast}/
            '';
            extensions = {
              enabled,
              all,
            }:
              enabled ++ [all.ast];
          };

        makePackage = php': let
          php = makePhp php';
        in
          pkgs.callPackage ./derivation.nix {
            inherit (php) buildPecl;
            inherit src php;
          };

        makeCheck = package:
          package.overrideAttrs (old: {
            doCheck = true;
          });

        pre-commit-check = pre-commit-hooks.lib.${system}.run {
          src = src';
          hooks = {
            actionlint.enable = true;
            alejandra.enable = true;
            alejandra.excludes = ["\/vendor\/"];
            clang-format.enable = true;
            clang-format.types_or = ["c" "c++"];
            clang-format.files = "\\.(c|h)$";
            markdownlint.enable = true;
            markdownlint.excludes = ["LICENSE\.md"];
            shellcheck.enable = true;
          };
        };
      in rec {
        packages = rec {
          php81 = makePackage pkgs.php81;
          php82 = makePackage pkgs.php82;
          default = php81;
        };

        devShells = pkgs.lib.mapAttrs (k: package:
          pkgs.mkShell {
            inputsFrom = [package];
            buildInputs = with pkgs; [
              actionlint
              clang-tools
              lcov
              package.php
              package.php.packages.composer
            ];
            shellHook = ''
              ${pre-commit-check.shellHook}
              export REPORT_EXIT_STATUS=1
              export NO_INTERACTION=1
              export PATH="$PWD/vendor/bin:$PATH"
            '';
          })
        packages;

        checks = {
          inherit pre-commit-check;
          php81 = makeCheck packages.php81;
          php82 = makeCheck packages.php82;
        };

        formatter = pkgs.alejandra;
      }
    );
}
