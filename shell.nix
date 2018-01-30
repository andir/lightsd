with import <nixpkgs> {}; {
  testEnv = (overrideCC stdenv gcc7).mkDerivation {
    name = "test";
    buildInputs = [ libyamlcpp websocketpp luajit_2_1 boost openssl ];
    nativeBuildInputs = [  clang pkgconfig gcc7 cmake ];
  };
}

