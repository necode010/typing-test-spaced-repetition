#include "windows.h"
#include "stdio.h"
#include "stdint.h"
#include "monkey_type_words.h"

#define CURTIME() ((float)(GetTickCount()) / 1000.0f)

#define MAX_WORDS 200

struct timing_t {
	float time_to_type;
	float time_since_typed;
};

struct timing_t timings[250] = {};

int sanity_check = 0;

#define TTT_1 0.6f
#define TTT_2 0.8f
#define TTT_3 1.5f

int words_typed = 0;

/*
    This function is my own attempt to copy Anki's flash card ordering algorithm,
    which is designed to make use of a psychological concept called spaced repetition for more efficient practice
    
    https://en.wikipedia.org/wiki/Anki_(software)
    https://en.wikipedia.org/wiki/Spaced_repetition
*/

int generate() {
	int r = rand() % MAX_WORDS;

	sanity_check += 1;
	if (sanity_check > 500)
		return r;

	struct timing_t* t = &timings[r];
	if (t->time_to_type == 0.0f)
		return r;

	if (t->time_to_type < TTT_1) {
		if (CURTIME() - t->time_since_typed > 600.0f)
			return r;
		//else
		//	printf("*avoided TTT1 %s*\n", words[r]);
	} else if (t->time_to_type < TTT_2) {
		if (CURTIME() - t->time_since_typed > 120.0f)
			return r;
		//else
		//	printf("*avoided TTT2 %s*\n", words[r]);
	} else if (t->time_to_type < TTT_3) {
		if (CURTIME() - t->time_since_typed > 60.0f)
			return r;
		//else
			//printf("*avoided TTT3 %s*\n", words[r]);
	}

	return generate();
}

int main() {
	int index = 0;
	float typing_begin = CURTIME();
    char correct = 0;
    
	srand(GetTickCount());
    
	while (1) {
		printf("\"%s\"\n", s_words[index % MAX_WORDS]);
        
		char text[128];
		fgets(text, sizeof(text), stdin);
        
		text[strlen(text) - 1] = '\0';
        
		correct = strcmp(text, s_words[index % MAX_WORDS]) & 1;
        
		if (correct == 0) {
			struct timing_t* t = &timings[index];
			t->time_to_type = CURTIME() - typing_begin;
			t->time_since_typed = CURTIME();
            
			float avg = 0.0f;
			float avg_index = 0.0f;
            
			for (int i = 0; i < MAX_WORDS; ++i) {
				if (timings[i].time_to_type != 0.0f) {
					avg += timings[i].time_to_type;
					avg_index += 1.0f;
				}
			}
            
			if (avg_index > 0.0f)
				avg /= avg_index;
            
			words_typed += 1;
            
			printf("typing completed : words_typed %i, t->time_to_type %f, avg %f\n", words_typed, t->time_to_type, avg);
            
			sanity_check = 0;
			index = generate() % MAX_WORDS;
            
			typing_begin = CURTIME();
		}
	}
    
	return 0;
}