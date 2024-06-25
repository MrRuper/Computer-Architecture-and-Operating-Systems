#include "nand.h" // Declaration of the library interface.
#include <errno.h> // For errno and ENOMEM.
#include <stdlib.h> // For malloc, calloc.
#include <limits.h> // For UINT_MAX value

// Macro representing logical gate without ports.
#define NO_PORTS 0

// Macro for counting maximum value.
#define max(x, y) (((x) >= (y)) ? (x) : (y))

struct Port;
struct Cable;

/**@brief This structure represents single logical gate.
 * ports                  - array of Port structure. Its length is represented
 *                          by the variable number_of_ports.
 * cables                 - array of Cable structure representing all nand-nand
 *                          connections. Its length is represented by the variable
 *                          length_of_cables.
 * number_of_ports        - unsigned integer representing the number of the gates input.
 * length_of_cables_array - unsigned integer representing the length of cables.
 * number_of_cables       - unsigned integer representing the true number of cables
 *                          in the logical gate. Because struct Cable* cables acts in the same
 *                          manner as vector in C++ we have number_of_cables <= length_of_cables_array.
  * my_longest_path       - ssize_t variable representing the longest path from this gate to the
  *                         boolean signal or to the gate without ports. Initial value is 0. Before
  *                         every nand_evaluate function call this variable is set to 0.
 * visited                - boolean technical variable created for DFS in nand_evaluate function.
 *                          Initially is set to false. Before every nand_evaluate this value is always false.
 * updated                - boolean technical variable supporting DFS and cycle detection in nand_evaluate.
 * gate_output_signal     - boolean variable describing the gate output during the nand_evaluate function.
 * any_false              - technical boolean variable which is true if some of the ports of this gate
 *                          keeps signal false and is false otherwise. This variable facilities rapid
 *                          computations of gate_output_signal.
 */
typedef struct nand {
    struct Port* ports;
    struct Cable* cables;
    unsigned int number_of_ports;
    unsigned int length_of_cables_array;
    unsigned int number_of_cables;
    ssize_t my_longest_path;
    bool visited;
    bool gate_output_signal;
    bool updated;
    bool any_false;
} nand_t;

/** @brief Structure which represents a single cable of logical gate.
 * port_number         - unsigned number representing the port
 *                       number of the gate which is linked by this cable.
 * linked_logical_gate - the address of the gate which is linked by this
 *                       cable.
 */
typedef struct Cable {
    unsigned port_number;
    nand_t* linked_logical_gate;
} cable_t;

/** @brief Structure which represents ports of the logical gate.
 * direct_signal  - if the signal is shared from a boolean signal
 *                  it will keep its address. Is whether the signal
 *                  is shared from another gate or there is no signal.
 * sharing_gate   - if an input signal is shared from another gate it
 *                  will hold its address. Is NULL whether there is no
 *                  income signal or signal is shared directly from a
 *                  boolean signal.
 * signal         - keeps the transferred signal
 *                  otherwise its behaviour is random,
 */
typedef struct Port {
    const bool* direct_signal;
    nand_t* sharing_gate;
    unsigned int cable_index;
} port_t;

nand_t* nand_new(unsigned n) {
    port_t* input_signal = NULL;
    nand_t* new_nand = NULL;

    if (n) {
        input_signal = (port_t*)malloc(n * sizeof(port_t));

        if (!input_signal) {
            errno = ENOMEM;
            return NULL;
        }

        for (unsigned i = 0; i < n; i++) {
            input_signal[i].direct_signal = NULL;
            input_signal[i].sharing_gate = NULL;
        }
    }

    new_nand = (nand_t*)malloc(sizeof(nand_t));

    if (!new_nand) {
        errno = ENOMEM;
        free(input_signal);
        input_signal = NULL;
        return NULL;
    }

    new_nand->ports = input_signal;
    new_nand->visited = false;
    new_nand->any_false = false;
    new_nand->updated = false;
    new_nand->my_longest_path = 0;
    new_nand->cables = NULL;
    new_nand->length_of_cables_array = 0;
    new_nand->number_of_cables = 0;
    new_nand->number_of_ports = n;
    return new_nand;
}

/**@brief Replace the last cable in the logical gate g with
 * removed one on the position i. Here the cable on the position
 * i is assumed to be already removed, thus the only thing need
 * to be done is replacing and updating information about new
 * cable index in another linked gate. Also updates the number of cables.
 * @param g - pointer to the logical gate.
 * @param i - index of already removed cable in g which
 *            where the last one has to be set.
 */
static void replace_deleted_cable_with_last_one(nand_t* g, unsigned int i) {
    unsigned int number_of_cables = g->number_of_cables - 1;
    unsigned int port_number = g->cables[number_of_cables].port_number;
    nand_t* linked_gate = g->cables[number_of_cables].linked_logical_gate;

    // Put last cable to the i-th cable.
    g->cables[i].linked_logical_gate = linked_gate;
    g->cables[i].port_number = port_number;
    g->cables[number_of_cables].linked_logical_gate = NULL;

    // Change information about this replacement in linked gate.
    linked_gate->ports[port_number].cable_index = i;

    // Update cable number.
    g->number_of_cables--;
}

void nand_delete(nand_t *g) {
    if (!g) {
        return;
    }

    cable_t* cable;
    port_t* port;
    nand_t* sharing_gate;
    unsigned int cable_index;

    // Firstly remove cables connected to my ports.
    for (unsigned i = 0; i < g->number_of_ports; i++) {
        port = g->ports + i;
        sharing_gate = port->sharing_gate;

        if (sharing_gate) {
            cable_index = port->cable_index;
            replace_deleted_cable_with_last_one(sharing_gate, cable_index);
        }
    }

    // Remove my cables.
    for (unsigned int i = 0; i < g->number_of_cables; i++) {
        cable = g->cables + i;

        if (cable && cable->linked_logical_gate) {
            port_t* ports = cable->linked_logical_gate->ports;
            ports[cable->port_number].sharing_gate = NULL;
        }
    }

    free(g->cables);
    free(g->ports);
    free(g);
}

/**@brief Joins g_out cable of index cable_index or a direct_signal with a port k of
 * the gate g_in. If the port k of g_in is already in use this function will take care of
 * safe removing the old cable (also updating sharing_gate).
 *
 * @param g_out         - if direct_signal is NULL then it describes the new sharing gate.
 * @param g_in          - pointer to the gate which has to be modified.
 * @param cable_index   - index of the cable in g_out which has to be connected to g_in.
 * @param k             - port number of g_in.
 * @param direct_signal - is NULL if g_out is not NULL. If is not NULL then keeps pointer
 *                      - to the boolean signal.
 */
static void remove_signal_and_update(nand_t* g_out,
                                     nand_t* g_in,
                                     const unsigned int cable_index,
                                     const unsigned int k,
                                     const bool* direct_signal) {
    port_t* g_in_port = g_in->ports + k;

    if (g_in_port->sharing_gate || g_in_port->direct_signal) {
        if (g_in_port->direct_signal) {
            g_in_port->sharing_gate = g_out;
            g_in_port->direct_signal = direct_signal;
            g_in_port->cable_index = cable_index;
        }
        else {
            nand_t* sharing_gate = g_in_port->sharing_gate;
            unsigned int remove_cable_index = g_in_port->cable_index;
            unsigned int last_cable_index_sharing_gate = sharing_gate->number_of_cables - 1;

            // Firstly plug in the new cable of g_out
            g_in_port->sharing_gate = g_out;
            g_in_port->cable_index = cable_index;
            g_in_port->direct_signal = direct_signal;

            // Replace last cable in the place of removed cable
            // in g_in.
            cable_t* last_cable_sharing_gate = &sharing_gate->cables[last_cable_index_sharing_gate];
            cable_t* remove_cable = &sharing_gate->cables[remove_cable_index];

            remove_cable->linked_logical_gate = last_cable_sharing_gate->linked_logical_gate;
            remove_cable->port_number = last_cable_sharing_gate->port_number;
            last_cable_sharing_gate->linked_logical_gate->ports[remove_cable->port_number].cable_index = remove_cable_index;
            last_cable_sharing_gate->linked_logical_gate = NULL;
            sharing_gate->number_of_cables--;
        }
    }
    else {
        g_in_port->sharing_gate = g_out;
        g_in_port->direct_signal = direct_signal;
        g_in_port->cable_index = cable_index;
    }
}

/**@brief Creates cable in g_out which connects this gate with
 * g_in in the port k.
 * @param g_out - sharing gate in which the cable has to be created.
 * @param g_in  - pointer to the logical gate which will be saved to the
 *                created cable information.
 * @param k     - unsigned integer describing the port number in g_in.
 * Return value is the index of new created cable.
 */
static unsigned int create_cable(nand_t* g_out, nand_t* g_in,
                                 unsigned k, bool* created) {
    unsigned int index_of_free_cable;

    if (g_out->number_of_cables < g_out->length_of_cables_array) {
        index_of_free_cable =  g_out->number_of_cables;
        g_out->cables[index_of_free_cable].linked_logical_gate = g_in;
        g_out->cables[index_of_free_cable].port_number = k;
        g_out->number_of_cables++;
        *created = true;
        return index_of_free_cable;
    }
    else {
        if (g_out->length_of_cables_array > UINT_MAX / 2) {
            *created = false;
            return 0;
        }

        index_of_free_cable = g_out->length_of_cables_array;
        unsigned int new_length = 2 * index_of_free_cable + 1;
        cable_t* new_cables = (cable_t*)calloc(new_length, sizeof(cable_t));

        if (!new_cables) {
            *created = false;
            return 0;
        }

        // Copy an old content.
        for (unsigned int i = 0; i < g_out->length_of_cables_array; i++) {
            new_cables[i].linked_logical_gate = g_out->cables[i].linked_logical_gate;
            new_cables[i].port_number = g_out->cables[i].port_number;
        }

        // The last one.
        new_cables[index_of_free_cable].linked_logical_gate = g_in;
        new_cables[index_of_free_cable].port_number = k;

        free(g_out->cables);
        g_out->cables = new_cables;
        g_out->length_of_cables_array = new_length;
        g_out->number_of_cables++;
        *created = true;
    }

    return index_of_free_cable;
}


int nand_connect_nand(nand_t *g_out, nand_t *g_in, unsigned k) {
    if (!g_out || !g_in || k >= g_in->number_of_ports) {
        errno = EINVAL;
        return -1;
    }

    // Create cable in g_out.
    bool created = false;
    unsigned int new_cable_index;
    new_cable_index = create_cable(g_out, g_in, k, &created);

    if (!created) {
        errno = ENOMEM;
        return -1;
    }

    remove_signal_and_update(g_out, g_in, new_cable_index, k, NULL);
    return 0;
}

int nand_connect_signal(bool const *s, nand_t *g, unsigned k) {
    if (!g || !s || k >= g->number_of_ports) {
        errno = EINVAL;
        return -1;
    }

    remove_signal_and_update(NULL, g, 0, k, s);
    return 0;
}

// Recursive function for setting visited gate g back to unvisited,
// not updated and with longest_path = 0.
static void change_values_back_to_false_recursive(nand_t* g) {
    port_t* port;

    if (!g) {
        return;
    }

    // Update my values back to the unvisited status.
    g->my_longest_path = 0;
    g->any_false = false;
    g->updated = false;
    g->visited = false;

    for (unsigned int i = 0; i < g->number_of_ports; i++) {
        port = g->ports + i;

        // If sharing gate was visited then go there recursively.
        if (port->sharing_gate && port->sharing_gate->visited) {
            change_values_back_to_false_recursive(port->sharing_gate);
        }
    }
}

// At the end of nand_evaluate or during nand_evaluate (if system is not
// correct) sets all technical variables of visited gates to the initial values.
static void change_values_back_to_false(nand_t** g, size_t index) {
    nand_t* gate;

    for (size_t i = 0; i <= index; i++) {
        gate = g[i];

        if (!gate) {
            continue;
        }

        change_values_back_to_false_recursive(gate);
    }
}

// Sets all technical variables of logical gate g to
// the initial values.
static void set_to_unvisited(nand_t* g) {
    g->visited = false;
    g->updated = false;
    g->any_false = false;
    g->my_longest_path = 0;
}

/**@brief This function processes a single gate during a recursive walkthrough of the system.
 * If system of gates is correct (i.e. there is no cycle, every nand is not NULL, every port is not empty)
 * it will update the gate g in the sense that g will keep correct values of gate_output_signal,
 * my_longest_path, visited, updated. Also if the system is correct the variable maximal_length
 * will keep the longest path in connected component of g "back" to the signal (back means
 * that we go the sharing_gate instead of linked_logical_gate). If system is not correct it will return
 * false and also will raise is_cycle in case of cycle error or empty_port in case of empty port of g.
 * @param g                 - pointer to the logical gate which needs to be processed.
 * @param maximum_length    - global maximum_length of whole system.
 * @param is_cycle          - technical pointer to boolean variable which is true if system
 *                            has cycle.
 * @param empty_port        - technical pointer to boolean variable which is true if some of
 *                            the ports of the g is empty.
 * @return true if the system "back" from g is correct and false otherwise.
 */
static bool recursive_gate(nand_t* g, ssize_t* maximum_length,
                           bool* is_cycle,
                           bool* empty_port) {
    port_t* port;
    g->visited = true;

    if (g->number_of_ports == NO_PORTS) {
        g->gate_output_signal = false;
        g->my_longest_path = 0;
    }

    for (unsigned int i = 0; i < g->number_of_ports; i++) {
        port = g->ports + i;

        if (port->direct_signal) { // Signal-nand connection.
            if (*port->direct_signal == false) {
                g->any_false = true;
            }

            g->my_longest_path = max(1, g->my_longest_path);
            *maximum_length = max(g->my_longest_path, *maximum_length);
        }
        else if (port->sharing_gate) { // Nand-nand connection.
            bool correct_system = true;

            // Cycle condition.
            if (port->sharing_gate->visited && !port->sharing_gate->updated) {
                set_to_unvisited(g);
                *is_cycle = true;
                return false;
            }
            if (!port->sharing_gate->visited) {
                correct_system = recursive_gate(port->sharing_gate,
                                                maximum_length,
                                                is_cycle, empty_port);
            }
            if (correct_system) {
                g->my_longest_path = max(port->sharing_gate->my_longest_path + 1, g->my_longest_path);

                if (port->sharing_gate->gate_output_signal == false) {
                    g->any_false = true;
                }
            }
            else { // System not correct
                set_to_unvisited(g);
                return false;
            }
        }
        else { // Empty port - no connection.
            set_to_unvisited(g);
            *empty_port = true;
            return false;
        }
    }

    g->updated = true;
    *maximum_length = max(g->my_longest_path, *maximum_length);

    if (g->any_false) {
        g->gate_output_signal = true;
    }
    else {
        g->gate_output_signal = false;
    }

    return true;
}

ssize_t nand_evaluate(nand_t **g, bool *s, size_t m) {
    if (!g || !s || m == 0) {
        errno = EINVAL;
        return -1;
    }

    nand_t *gate;
    bool correct_system = true;
    bool is_cycle = false;
    bool empty_port = false;
    ssize_t maximum_length = -1;

    for (size_t i = 0; i < m; i++) {
        gate = g[i];

        if (gate && !gate->visited) {
            correct_system = recursive_gate(gate, &maximum_length, &is_cycle, &empty_port);
        }
        else if (!gate) {
            if (i > 0) {
                change_values_back_to_false(g, i - 1);
            }

            errno = EINVAL;
            return -1;
        }
        if (correct_system) {
            s[i] = gate->gate_output_signal;
        }
        else {
            if (is_cycle) {
                errno = ECANCELED;

                if (i > 0) {
                    change_values_back_to_false(g, i - 1);
                }
            }
            else if (empty_port) {
                errno = ECANCELED;
                change_values_back_to_false(g, i);
            }

            return -1;
        }
    }

    change_values_back_to_false(g, m - 1);
    return maximum_length;
}
ssize_t nand_fan_out(nand_t const *g) {
    if (!g) {
        errno = EINVAL;
        return -1;
    }

    ssize_t answer = (ssize_t)(g->number_of_cables);
    return answer;
}

void* nand_input(nand_t const *g, unsigned k) {
    if (!g || k >= g->number_of_ports) {
        errno = EINVAL;
        return NULL;
    }

    port_t* port = g->ports + k;

    if (!port->sharing_gate && !port->direct_signal) {
        errno = 0;
        return NULL;
    }
    if (port->sharing_gate) {
        return port->sharing_gate;
    }

    return (void*)port->direct_signal;
}

nand_t* nand_output(nand_t const *g, ssize_t k) {
    if (!g || k >= g->number_of_cables) {
        errno = EINVAL;
        return NULL;
    }

    return g->cables[k].linked_logical_gate;
}
