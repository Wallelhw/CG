#include <iostream>
#include <vector>

#include "CGL/vector2D.h"

#include "mass.h"
#include "rope.h"
#include "spring.h"

namespace CGL {

    Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass, float k, vector<int> pinned_nodes)
    {
        // TODO (Part 1): Create a rope starting at `start`, ending at `end`, and containing `num_nodes` nodes.

        num_nodes = num_nodes > 1 ? num_nodes : 2;
        Vector2D spring_distance = (end - start)/ (num_nodes - 1);
        
        for(int i = 0; i < num_nodes; i++){
            masses.push_back(new Mass( start + i * spring_distance, node_mass, false));
        }
        for(int i = 0; i < masses.size() - 1; i++){
            springs.push_back(new Spring( masses[i], masses[i + 1], k));
        }

       for (auto &i : pinned_nodes) {
           masses[i]->pinned = true;
       }
    }

    void Rope::simulateEuler(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            // TODO (Part 2): Use Hooke's law to calculate the force on a node
            auto m1tom2 = s->m2->position - s->m1->position;
            auto raletive_position =  m1tom2.norm() - s->rest_length; //use for calculating elastic force
            auto raletive_velocity =dot(s->m1->velocity - s->m2->velocity,m1tom2/ m1tom2.norm()) * m1tom2/ m1tom2.norm(); //use for calculating damp
            auto raletive_velocity_t = raletive_velocity - (s->m1->velocity - s->m2->velocity);   //simulate torque damp

            if (raletive_position > 0.2f * s->rest_length) raletive_position = s->rest_length;
            if(!s->m1->pinned){
                s->m1->forces += s->k * m1tom2 / m1tom2.norm() * raletive_position - raletive_velocity * 0.05 + raletive_velocity_t * 0.05;
            }
                
            if(!s->m2->pinned){
                s->m2->forces += -s->k * m1tom2 / m1tom2.norm() * raletive_position + raletive_velocity * 0.05 - raletive_velocity_t * 0.05;
            }
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                // TODO (Part 2): Add the force due to gravity, then compute the new velocity and position
                // TODO (Part 2): Add global damping
                m->forces += gravity * m -> mass;
                m->velocity += delta_t * (m->forces / m->mass);     //implicit euler method (order 1)
                m->position += delta_t * m->velocity ;               //local truncation error:O(h2)
            }
            // Reset all forces on each mass
            m->forces = Vector2D(0, 0);
        }
    }

    void Rope::simulateVerlet(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            // TODO (Part 2): Use Hooke's law to calculate the force on a node
            auto m1tom2 = s->m2->position - s->m1->position;
            auto raletive_position =  m1tom2.norm() - s->rest_length; //use for calculating elastic force
            auto raletive_velocity =dot(s->m1->velocity - s->m2->velocity,m1tom2/ m1tom2.norm()) * m1tom2/ m1tom2.norm(); //use for calculating damp
            auto raletive_velocity_t = raletive_velocity - (s->m1->velocity - s->m2->velocity);   //simulate torque damp

            if (raletive_position > 0.2f * s->rest_length) raletive_position = s->rest_length;
            if(!s->m1->pinned){
                s->m1->forces += s->k * m1tom2 / m1tom2.norm() * raletive_position - raletive_velocity * 0.05 + raletive_velocity_t * 0.05;
            }
                
            if(!s->m2->pinned){
                s->m2->forces += -s->k * m1tom2 / m1tom2.norm() * raletive_position + raletive_velocity * 0.05 - raletive_velocity_t * 0.05;
            }
        }

        for (auto &m : masses)
        {
            
            if (!m->pinned)
            {
                // TODO (Part 3.1): Set the new position of the rope mass
                auto temp = m->position;
                m->forces += gravity * m -> mass;
                m->position = 2 * m->position - m->last_position + (m->forces / m->mass) * delta_t * delta_t;
                
                m->velocity += delta_t * (m->forces / m->mass);
                m->last_position = temp;
                // TODO (Part 4): Add global Verlet damping


            }
            m->forces = Vector2D(0, 0);
        }

    }
}
