{ lib
, stdenv

# , pkgs
, python3
, pkgconfig
, gettext
, libusb1
, hidapi
, autoreconfHook
}:
stdenv.mkDerivation {
	pname = "libx52";
	version = lib.strings.removeSuffix "\n" (builtins.readFile ./Version);

	src = builtins.path { path = ./.; name = "libx52"; };

	"move-systemd-user-units.sh" = true;
	configureFlags = [
		# "--disable-systemd"
		"--with-systemdsystemunitdir=${placeholder "out"}/lib/systemd/system/"
		"--with-udevrulesdir=${placeholder "out"}/lib/udev/rules.d"
	];
	nativeBuildInputs = builtins.attrValues { 
		inherit
			python3
			pkgconfig;
	};

	buildInputs = builtins.attrValues { 
		inherit
			gettext
			libusb1
			hidapi

			autoreconfHook;
	};

}