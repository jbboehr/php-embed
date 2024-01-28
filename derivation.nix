{
  lib,
  php,
  stdenv,
  autoreconfHook,
  buildPecl,
  src,
}:
buildPecl rec {
  pname = "embed";
  name = "embed-${version}";
  version = "v0.1.0";

  inherit src;

  passthru.php = php;

  makeFlags = ["phpincludedir=$(out)/include/php"];

  checkTarget = "test";
  checkFlags = ["REPORT_EXIT_STATUS=1" "NO_INTERACTION=1"];
}