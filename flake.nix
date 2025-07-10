{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    utils,
  }:
    utils.lib.eachDefaultSystem
    (system: let
      pkgs = import nixpkgs {
        inherit system;
      };
      i686cross = pkgs.pkgsCross.i686-embedded;
    in {
      devShells.default = pkgs.mkShell {
        packages = with pkgs; [
          i686cross.buildPackages.gcc
        ];
      };

      packages = {
        mageos = i686cross.stdenv.mkDerivation {
          name = "mageos";
          src = self;
          # TODO Setup build commands
        };
      };
    });
}
