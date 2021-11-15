{
	description = "A very basic flake";
	inputs.utils.url = "github:numtide/flake-utils";

	outputs = { self, nixpkgs, utils }: {} // utils.lib.eachDefaultSystem 
	(system: let pkgs = import nixpkgs { inherit system; }; in {

		packages.libx52 = pkgs.callPackage ./default.nix {};

		defaultPackage = self.packages.${system}.libx52;

	});
}
