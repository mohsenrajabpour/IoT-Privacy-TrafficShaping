# Development and Assessment of Traffic Shaping Systems for Privacy Improvement in IoT Networks

This repository contains the implementation and development resources for the publication "Development and Assessment of Traffic Shaping Systems for Privacy Improvement in IoT Networks". 

The project involves developing traffic shaping mechanisms using the capabilities of the Click Modular Router (https://github.com/kohler/click), where newly created elements can be added to Click to implement the traffic shaping systems.

## About This Repository
This repository hosts new elements developed using the Click Modular Router, tailored for our research conducted in the aforementioned publication. The practical development, implementation, and tests were conducted on the [ORBIT testbed](https://www.orbit-lab.org).

### Experiment Setup
The experiments were conducted on the SB4 of the ORBIT testbed. The files located in the `click_elements` directory should be placed in the `/click/elements/local` directory of the Click Modular Router.

### Compiling the Click Elements
To compile the new elements:
1. Navigate to the click directory: `cd click`
2. Generate the element list: `make elemlist`
3. Compile the Click router: `make`
4. Install the compiled elements: `make install all`

### Running the Experiment
1. Execute `load_images.sh` to load necessary images on the testbed nodes.
2. Run `setup_nodes.sh` to prepare the nodes for the experiment.
3. Launch the routers and stations using `start_router.sh`.
    - This step also transfers necessary files to stations, Access Point (AP), and server, such as `run_tcpreplay.sh`, `ap.click`, `se.click`, and `*.pcap` files.
4. At the AP and server, run `ap.click` and `se.click` respectively.
5. Execute `run_tcpreplay.sh` at the stations to initiate traffic replay.
