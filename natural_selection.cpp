#include <iostream>
#include <vector>
#include <random>
#include <unistd.h>

using namespace std;

#define BOARD_SIZE 30
#define FOOD_ENERGY 10
#define MAX_ENERGY 30

random_device dev;
mt19937 rng(dev());
uniform_int_distribution<mt19937::result_type> dist(0,BOARD_SIZE-1);

int rand() {
	return dist(rng);
}

template<typename T>
void remove(std::vector<T>& vector, T const& value){
    vector.erase(std::remove(begin(vector), end(vector), value), end(vector));
}

struct Point {
    int x;
    int y;
    
    Point(int x = 0, int y = 0) {
    	this->x = x;
    	this->y = y;
    }

    inline Point& operator=(const Point& other){
        x = other.x;
        y = other.y;
        return *this;
    }
    
    inline Point& operator+=(const Point& other){
        x += other.x;
        y += other.y;
        return *this;
    }
    
    inline Point operator+(const Point& other) const {
        Point res {x+other.x,y+other.y};
        return res;
    }

    const inline bool operator==(const Point& other) const {
        return (x==other.x && y == other.y);
    }
};

class Creature {
		private:
			static int sequency;
			Point old_pos;

		public:
			int id;
			Point pos;
			int energy = MAX_ENERGY;
		
			Creature(int x, int y) {
				pos = {x,y};
				id = ++sequency;
			}
		
			string color() {
				double e = energy / (double)MAX_ENERGY;
				return to_string(41 + min((int)(e * 7), 6));
			}
		
			void eat() {
				energy += FOOD_ENERGY;
			}
			
			void move(Point to) {
				old_pos = pos;
				pos = to;
				energy--;
			}
			
			void move() {
				Point npos;
				npos.x += rand() % 3 - 1;
				npos.y += rand() % 3 - 1;
				
				int tx = pos.x + npos.x;
				if(tx >= BOARD_SIZE || tx < 0) {
					npos.x *= -1;
				}
				
				int ty = pos.y + npos.y;
				if(ty >= BOARD_SIZE || ty < 0) {
					npos.y *= -1;
				}
				
				if(pos + npos == old_pos) {
					move();
					return;
				}
				
				old_pos = pos;
				
				pos += npos;
				
				energy--;
			}
			
			const inline bool operator==(const Creature& other) const {
        return (id==other.id && id == other.id);
    	}
};

int Creature::sequency = 0;

class Board {

	private:
		vector<Creature> creatures;
		vector<Point> foods;
	
	public:
		Board(int count) {
			for(int i=0;i<count;i++) {
				Point p = randPoint();
				Creature c(p.x, p.y);
				creatures.push_back(c);
			}
			
			for(int i=0;i<count*10;i++) {
				Point p = randPoint();
				foods.push_back(p);
			}
		}
		
		Point randPoint() {
			int x = rand() % BOARD_SIZE;
			int y = rand() % BOARD_SIZE;
			return {x,y};
		}
	
		void print() {
			system("clear");
			
			for(int i=0 ; i<BOARD_SIZE ; i++) {
				for(int j=0 ; j<BOARD_SIZE ; j++) {
					Point p = {j,i};
					Creature *cr = NULL;
					
					for (auto &c : creatures) {
						if(c.pos == p) {
							cr = &c;
							break;
						}
					}
					
					bool food = false;
					
					for (auto &f : foods) {
						if(f == p) {
							food = true;
						}
					}
					
					if(cr != NULL) {
						cout << "\x1b[1;" << cr->color() << "m " << cr->id << (cr->id > 9 ? "" : " ") << "\x1b[0m";
					} else if(food) {
						cout << "\x1b[33m * \x1b[0m";
					} else {
						cout << "   ";
					}
				}
				cout << endl;
			}
		}
		
		void move() {
			for (auto &c : creatures) {
				Point *food = NULL;
			
				// search near food
				for (auto &f : foods) {
					for(int i=-1;i<=1 && food==NULL;i++) {
						for(int j=-1;j<=1 && food==NULL;j++) {
							if(c.pos.x+j == f.x && c.pos.y+i == f.y) {
								food = &f;
							}
						}
					}
				}
				
				// if found, move towards the food
				if(food != NULL) {
					c.move(*food);
				} else {
					c.move();
				}
				
				// check if creature should eat a food
				for (auto &f : foods) {
					if(c.pos == f) {
						c.eat();
						remove(foods, f);
						break;
					}
				}
				
				// kill creature
				if(c.energy == 0) {
					remove(creatures, c);
				}
			}
		}
};

int main() {

	Board board(8);
	
	while(true) {
		board.move();
		board.print();
		usleep(300000);
	}

	return 0;
}