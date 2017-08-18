lightsd
====

lightsd implements a sequence based approach towards LED coloring.
The daemon continuously runs through the configured sequence at the given frame rate.

During each render run all `Operations` within the `Sequence` are called. An `Operation` can either be enabled (`1`) or disabled (`0`).

Runtime configuration changes are accepted via mqtt. The mqtt broker can be configured in `config.yml`.

An example `config.yml` is in this repository.


Building / Development / Dependencies
===

The main dependencies are

- g++ (recent versions, tested with >=6)
- libwebsocketcpp-dev (for preview via websockets, see [lightsd-vis](https://github.com/flokli/lightsd-vis/))
- libyaml-cpp-dev (for configuration parsing)
- libssl-dev (for the websocket stuff etc..)
- libluajit-5.1-dev (for the lua plugin api)

The build process is run by `cmake`. Consult the [gitlab-ci configuration](.gitlab-ci.yml) to see the full build process.