#define PERSON_BUFFER 100
#define NUM_PEOPLE    10000
#define DAYS_INFECTED 10
#define EXP_PARAMETER 90.0
#define POW_ALPHA     5
#define POW_R_0       60.0
#define SUSCEPTIBLE   0
#define RECOVERED     -1
class World
{
public:
  typedef struct person {
    // 0 = Susceptible
    // 1 - 254 = Infected
    // 255 = Dead/safe/recovered
    char status;
    float x;
    float y;
  } person;

  World();
  virtual ~World();

  person* get_people();
  int print_people(person[]);
  bool step();

private:
  int generate_people(person **, int);
  int populate_person(person *);
  int populate_people(person *, int);
  int print_person(char *, person *);
  bool contact_occurs(person, person);
  float toroidal_distance(float, float, float, float);
  int infect(int);
  int cure(int);
  int progress_sickness(int);

  person* master_list;
  int master_count;
  // Numbers of people infected, susceptible, or recovered
  int num_i;
  int num_s;
  int num_r;
};
