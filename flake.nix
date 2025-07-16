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
      x86_64-cross = pkgs.pkgsCross.x86_64-embedded;
    in {
      devShells.default = pkgs.mkShell {
        packages = with pkgs; [
          x86_64-cross.buildPackages.gcc
          # gcc
        ];
      };

      packages = {
        mageos = x86_64-cross.stdenv.mkDerivation {
          name = "mageos";
          src = self;
          # TODO Setup build commands
        };
      };
    });
}
