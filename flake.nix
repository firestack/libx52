{
  description = "A very basic flake";
  inputs.utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, utils }: {} // utils.lib.eachDefaultSystem (system: let pkgs = import nixpkgs {inherit system; }; in {

    packages.libx52 = pkgs.stdenv.mkDerivation {
      pname = "libx52";
      version = nixpkgs.lib.strings.removeSuffix "\n" (builtins.readFile ./Version);
      src = builtins.path { path = ./.; name = "libx52"; };

      "move-systemd-user-units.sh" = true;
      configureFlags = [
        # "--disable-systemd"
        "--with-systemdsystemunitdir=${placeholder "out"}/lib/systemd/system/"
        "--with-udevrulesdir=${placeholder "out"}/lib/udev/rules.d"
      ];
      nativeBuildInputs = builtins.attrValues { 
        inherit (pkgs)
          python3
          pkgconfig;
      };

      buildInputs = builtins.attrValues { 
        inherit (pkgs)
          gettext
          libusb1
          hidapi

          autoreconfHook;
      };

    };

    defaultPackage = self.packages.${system}.libx52;

  });
}
