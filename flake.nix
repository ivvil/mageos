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
      cross = pkgs.pkgsCross.i686-embedded;
    in {
      devShells.default = pkgs.mkShell {
        packages = with pkgs; [
          cross.buildPackages.clang
          # gcc
        ];
      };

      packages = {
        mageos = cross.stdenv.mkDerivation {
          name = "mageos";
          src = self;
          # TODO Setup build commands
        };
      };
    });
}
