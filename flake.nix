{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs =
    { self, nixpkgs, ... }:
    let
      system = "x86_64-linux";
    in
    {
      devShells.${system}.default =
        let
          pkgs = import nixpkgs { inherit system; };
        in
        pkgs.mkShell {

          nativeBuildInputs = with pkgs; [
            vulkan-tools
            glslang
            pkg-config
          ];

          buildInputs = with pkgs; [
            vulkan-headers
            vulkan-loader
            vulkan-validation-layers
            glfw
          ];

          shellHook = ''
            export VULKAN_SDK="${pkgs.vulkan-headers}"

            export VK_LAYER_PATH="${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d"          '';
        };
    };
}
