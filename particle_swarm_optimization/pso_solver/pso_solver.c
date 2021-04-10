/* ===========================================================
   #File: pso_solver.c #
   #Date: 11 Apr 2021 #
   #Revision: 1.0 #
   #Creator: Omid Miresmaeili #
   #Description: A simple particle swarm optimization (PSO) #
   #Notice: (C) Copyright 2021 by Omid. All Rights Reserved. #
   =========================================================== */

#include <stdio.h>
#include <stdlib.h> // rand()
#include <time.h>   // time(NULL)
#include <float.h>  // FLT_MAX
#include <memory.h> // memset

#include "ObjectiveFunctions.h"

#define NUM_VAR     3
#define MAX_ITER    1000
#define NUM_POP     200

typedef float CostFunction (float x [], size_t num_var, int * nfe);
CostFunction * cost_fn = &sphere;

float lb[NUM_VAR] = {1.0f, -3.0f, -5.0f};
float ub[NUM_VAR] = {4.0f, -1.0f,  5.0f};

float w = 1.0f;
float wdamp = 0.99f;
float c1 = 2.0f;
float c2 = 2.0f;

// Returns: a random real number in the interval [0,1]
static float
get_randf () {
    return (float)rand() / (float)RAND_MAX;
}
// Returns: a random real number in the interval [a,b]
static float
get_randf_ex (float a, float b) {
    return a + (b - a) * get_randf();
}
int
main (void) {
    srand((unsigned)time(NULL)); // randomize seed

    float alpha = 0.1f;
    float velocity_max[NUM_VAR];
    float velocity_min[NUM_VAR];

    for (unsigned i = 0; i < NUM_VAR; ++i) {
        velocity_max[i] = alpha * (ub[i] - lb[i]);
        velocity_min[i] = -velocity_max[i];
    }

    typedef struct {
        float position[NUM_VAR];
        float cost;
    } Best;
    typedef struct {
        float position[NUM_VAR];
        float velocity[NUM_VAR];
        float cost;
        Best best;
    } Particle;
    Particle * particles = (Particle *)calloc(NUM_POP, sizeof(Particle));

    static int nfe = 0;
    Best global_best = {.cost = FLT_MAX};
    //
    // Particles Initialization
    //
    for (unsigned i = 0; i < NUM_POP; ++i) {
        for (unsigned j = 0; j < NUM_VAR; ++j)
            particles[i].position[j] = get_randf_ex(lb[j], ub[j]);
        memset(particles[i].velocity, 0, sizeof(particles[i].velocity));
        particles[i].cost = (*cost_fn)(particles[i].position, NUM_VAR, &nfe);
        memcpy(
            &particles[i].best.position,
            &particles[i].position,
            sizeof(particles[i].best.position)
        );
        particles[i].best.cost = particles[i].cost;
        if (particles[i].best.cost < global_best.cost)
            global_best = particles[i].best;
    }
    float best_costs[MAX_ITER];
    unsigned n_fevals[MAX_ITER];
    //
    // Main Loop
    //
    for (unsigned iter = 0; iter < MAX_ITER; ++iter) {
        for (unsigned i = 0; i < NUM_POP; ++i) {
            for (unsigned j = 0; j < NUM_VAR; ++j) {
                // Calculate velocity
                particles[i].velocity[j] =
                    w * particles[i].velocity[j] +
                    get_randf() * c1 * (particles[i].best.position[j] - particles[i].position[j]) +
                    get_randf() * c2 * (global_best.position[j] - particles[i].position[j]);
                // Velocity bounds
                particles[i].velocity[j] = __max(particles[i].velocity[j], velocity_min[j]);
                particles[i].velocity[j] = __min(particles[i].velocity[j], velocity_max[j]);
                // Update position
                particles[i].position[j] +=  particles[i].velocity[j];
                // Mirror velocity if needed
                if (
                    particles[i].position[j] > ub[j] ||
                    particles[i].position[j] < lb[j]
                    ) {
                    particles[i].velocity[j] *= -1;
                }
            }
            // Cost function evaluation (update bests if needed)
            particles[i].cost = (*cost_fn)(particles[i].position, NUM_VAR, &nfe);
            if (particles[i].cost < particles[i].best.cost) {
                particles[i].best.cost = particles[i].cost;
                memcpy(
                    &particles[i].best.position,
                    &particles[i].position,
                    sizeof(particles[i].best.position)
                );
                if (particles[i].best.cost < global_best.cost)
                    global_best = particles[i].best;
            }
        }
        // End of iteration calculations
        best_costs[iter] = global_best.cost;
        n_fevals[iter] = nfe;
        printf(
            "Iteration %3u: NFE = %6d, Global Best Cost = %.8f\n",
            iter, n_fevals[iter],
            global_best.cost
        );
        w *= wdamp;
    }
    free(particles);
    return (0);
}
