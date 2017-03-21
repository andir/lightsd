lightsd
====

lightsd implements a sequence based approach towards LED coloring.
The daemon continuously runs through the configured sequence at the given frame rate.

During each render run all `Operations` within the `Sequence` are called. An `Operation` can either be enabled (`1`) or disabled (`0`).

Runtime configuration changes are accepted via mqtt. The mqtt broker can be configured in `config.yml`.

An example `config.yml` is in this repository.