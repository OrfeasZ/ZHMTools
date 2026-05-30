{
  description = "ZHMTools development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    fenix = {
      url = "github:nix-community/fenix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, flake-utils, fenix }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        llvm = pkgs.llvmPackages;

        fenixPkgs = fenix.packages.${system};
        rustToolchain = fenixPkgs.combine [
          fenixPkgs.complete.rustc
          fenixPkgs.complete.cargo
          fenixPkgs.complete.rust-src
          fenixPkgs.complete.clippy
          fenixPkgs.complete.rustfmt
          fenixPkgs.targets.x86_64-pc-windows-msvc.latest.rust-std
        ];

        commonNativeBuildInputs = [
          pkgs.cmake
          pkgs.ninja
          pkgs.pkg-config
          pkgs.git
          rustToolchain
        ];

        mkShellHook = stdenv: ''
          export CC="${stdenv.cc}/bin/${stdenv.cc.targetPrefix}cc"
          export CXX="${stdenv.cc}/bin/${stdenv.cc.targetPrefix}c++"
        '';
      in
      {
        devShells = {
          default = (pkgs.mkShell.override { stdenv = llvm.libcxxStdenv; }) {
            nativeBuildInputs = commonNativeBuildInputs ++ [
              llvm.clang-tools # clangd, clang-format, clang-tidy
              llvm.lldb
            ];
            shellHook = mkShellHook llvm.libcxxStdenv;
          };
        };
      });
}
