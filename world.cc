#include "world.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

// In the constructor of a world the list of people is generated.
World::World() {
	srand(time(NULL));
  master_list = (person *) malloc(NUM_PEOPLE * sizeof(person));
  master_count = generate_people(&master_list, NUM_PEOPLE);
  num_i = 0;
  num_s = master_count;
  num_r = 0;
  infect(0);
}

World::~World() {
  free(master_list);
}

// Returns all people in the world
World::person* World::get_people() {
  return master_list;
}
	
// Creates a list of n people for the world, currently uniformly
// randomly distributed on a (0, 1) torus.
int World::generate_people(person **all_people, int n) {
  int count = populate_people(*all_people, n);
	//int i;
	//for(i = 0; i < n; i++) {
		//populate_person(&all_people[i]);
	//}
  
	return count;
}

// Infect the person in master_list at given index and update counters
int World::infect(int index) {
  int first_susceptible = num_r + num_i;
  assert(index >= first_susceptible);
  assert(master_list[first_susceptible].status == SUSCEPTIBLE);

  // Switch places to maintain list order, if necessary.
  if(first_susceptible != index) {
    person temp = master_list[first_susceptible];
    master_list[first_susceptible] = master_list[index];
    master_list[index] = temp;
  }

  master_list[first_susceptible].status = DAYS_INFECTED;
  num_i++;
  num_s--;
  return 0;
}

// Cure the person, but provide the index in i_list
// We will have problems if people start to not recover in
// monotonic increasing order
int World::cure(int index) {
  int first_infected = num_r;
  int first_susceptible = num_r + num_i;
  assert(index >= first_infected && index < first_susceptible);

  // Switch places to maintain list order, if necessary.
  if(first_infected != index) {
    person temp = master_list[first_infected];
    master_list[first_infected] = master_list[index];
    master_list[index] = temp;
  }

  master_list[first_infected].status = RECOVERED;
  num_r++;
  num_i--;
  return 0;
}

int World::progress_sickness(int index) {
  //people don't get better, so we are under the SI model
  //master_list[index].status--;
  if(master_list[index].status == SUSCEPTIBLE)
    cure(index);
}


// Prints all people in the world to the terminal
int World::print_people(person* list) {
  int i;
  char buffer[PERSON_BUFFER];

  for(i = 0; i < master_count; i++) {
    printf("%d\n", i);
		print_person(buffer, &list[i]);
		printf("%s\n", buffer);
	}
	
	return 0;
}
	
// Creates a person with a random location between 0 and 1
int World::populate_person(person * p) {
	p->status = 0;
	p->x = rand()/((float)RAND_MAX + 1);
	p->y = rand()/((float)RAND_MAX + 1);
	return 0;
}

int World::populate_people(person *people, int n) {
  ifstream file("data31.txt", ifstream::in);
  //these are hardcoded for now, maybe they should be read from the file in the future
  int xdim = 201;
  int ydim = 201;
  char value[30];
  int i;
  int j = 0;
	for(i = 0; i < n; i++) {
    people[j].status = 0;
    file.getline(value, 30, ' ');
    people[j].x = atof(value);
    file.getline(value, 30, '\n');
    people[j].y = atof(value);
    //By incrementing j every other time we halve the data size
    if(3 == i%4) {
      j++;
    }
  }
  printf("%d\n", j);
  return j;
}

// Takes a person and generates a string representing that person
int World::print_person(char *str, person *p) {
	sprintf(str, "%d, (%f, %f)", p->status, p->x, p->y);
	return 0;
}

// Progress the world one step, propegating infection status when appropriate
bool World::step() {
  assert(num_s + num_i + num_r == master_count);
  int old_num_i = num_i;
  int old_num_r = num_r;
  for(int i = num_r; i < old_num_r + old_num_i; i++) {
    int old_num_s = num_s;
    for(int j = num_r + num_i; j < master_count; j++) {
      float r = rand()/((float)RAND_MAX + 1);
      if(i == j) {
        assert(false);
      }
      assert(i != j);
      assert(master_list[j].status == SUSCEPTIBLE && master_list[i].status > SUSCEPTIBLE);
      if(contact_occurs(master_list[i], master_list[j]))
        infect(j);
    }

    progress_sickness(i);
  }
  return (num_i != 0);
}



// Randomly simulate contact between people. People are more likely to
// come into contact if they are closer to each other.
bool World::contact_occurs(person a, person b) {
  float d = toroidal_distance(a.x, b.x, a.y, b.y);

  // uniform random variable in (0, 1)
  float u = rand()/((float)RAND_MAX + 1);

  // exponential random variable
  //float e = exp(-EXP_PARAMETER * d);

  // power law random variable
  float p = pow(1 + pow(d*POW_R_0, POW_ALPHA), -1);
  return u < p;
}

// Returns distance between two points on a plane, where the edges of the plane
// are assumed to be "wrapped" to the other side.
float World::toroidal_distance(float x1, float x2, float y1, float y2) {
  float dx = x1 - x2;
  float dy = y1 - y2;
  if(dx < 0)
    dx = dx * -1.0;
  if(dy < 0)
    dy = dy * -1.0;

  if(dx > .5)
    dx = 1.0 - dx;
  if(dy > .5)
    dy = 1.0 - dy;

  // toroidal distance between people (max = sqrt(2)/2)
  return sqrt(dx*dx + dy*dy);
}
