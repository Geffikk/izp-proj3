/*---------------------------------------------
- FILE : 	proj3.c
- DATE : 	December 2018
- AUTHOR :	xgeffe00@stud.fit.vutbr.cz
- PROJEKT :	Projekt 3 : Jednoducha shlukova analyza 
---------------------------------------------*/

/**
 * Kostra programu pro 3. projekt IZP 2018/19
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif



/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.

 *****************************************************************/



/*********************** Global variable *************************/

const int CLUSTER_CHUNK = 10;

/*****************************************************************/



/***************** ALL FUNCTIONS IN PROGRAM **********************/
/*
void sort_cluster(struct cluster_t *c);
void init_cluster(struct cluster_t *c, int cap);
void clear_cluster(struct cluster_t *c);
void append_cluster(struct cluster_t *c, struct obj_t obj);
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);
static int obj_sort_compar(const void *a, const void *b);
void print_cluster(struct cluster_t *c);
void init_clusters(struct cluster_t **carr, const int narr);
void clear_clusters(struct cluster_t *carr, const int narr);
int remove_cluster(struct cluster_t *carr, int narr, int idx);
float obj_distance(struct obj_t *o1, struct obj_t *o2);
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);
int load_clusters(char *filename, struct cluster_t **arr);
void print_clusters(struct cluster_t *carr, int narr);
int get_required_size_of_clusters(struct cluster_t *clusters, int size, const int required_size);
bool process_input_args (int argc, char *argv[]);
*/

/*****************************************************************/



/************************ BASIC STRUCUTURES **********************/

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************/





/***************** Work with Clusters *****************************/

//Calling function for sorting clusters
void sort_cluster(struct cluster_t *c);



/* Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.

 - 'c' zhluk pre inicializaciu
 - 'cap' kapacita zhluku */

void init_cluster(struct cluster_t *c, int cap) {

	assert(c);
	assert(cap >= 0);
	
	c->size = 0;
	
	if (cap > 0) {
		//Alocation memory for array object
		c->obj = malloc(cap * sizeof(struct obj_t));
		if (c->obj == NULL) {
			fprintf(stderr, "Wrong alocation memory");
		}
		c->capacity = cap;
		return;
		
	}
	
	//Alocation error or capacity is zero
	c->capacity = 0;
	c->obj = NULL; 	
}



/* Odstraneni vsech objektu shluku a inicializace na prazdny shluk */

void clear_cluster(struct cluster_t *c) {
	
	assert(c);
	
	free(c->obj);

	init_cluster(c,0); 
}



/* Zmena kapacity shluku 'c' na kapacitu 'new_cap' */

struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap) {
   
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL) {
    	printf("Wrong expand capacity cluster");
        return NULL;
    }

    c->obj = arr;
    c->capacity = new_cap;
    return c;
}



/* Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde. */

void append_cluster(struct cluster_t *c, struct obj_t obj) {
	
	assert(c); 
	assert(c->size >= 0);
	
	if (c->capacity <= c->size) {
		//expand capacity cluster
		if (! resize_cluster(c,c->capacity + CLUSTER_CHUNK)) {
			return;
		}
	}

	assert(c->obj); 
	
	//add obj on the first free position and increase value variable in cluster
	c->obj[c->size++] = obj;
}



/* Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen */

void merge_clusters(struct cluster_t *c1, struct cluster_t *c2) {
	
	assert(c1);
	assert(c2);
	assert(c2->size >= 0);

	if (c2->size != 0) {
		assert(c2->obj);
	}
	
	int previous_c2_size = c2->size;
	int previous_c1_size = c1->size;
	//add object cluster c2 to cluster c1
	for(int i=0; i < c2->size; i++) {
		append_cluster(c1,c2->obj[i]);
	}

	if (c2->size == previous_c2_size && c1->size == previous_c1_size + c2->size) {
		//sorting object in cluster 
		sort_cluster(c1);
	}
}
 


static int obj_sort_compar(const void *a, const void *b) {
	
	const struct obj_t *o1 = a;
	const struct obj_t *o2 = b;
	if (o1->id < o2->id) return -1;
	if (o1->id > o2->id) return 1;
	return 0;
}



void sort_cluster(struct cluster_t *c) {
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}



/**********************************************************************/
/* Prace s polem shluku */

void print_cluster(struct cluster_t *c) {
    
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}



/*Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.*/

int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
	assert(carr);
	assert(idx >= 0);
	assert(idx < narr);
	assert(narr > 0);

	
	//new size will be 1 less 
	int new_narr = narr - 1;
	//clear memory for cluster on index idx 
	clear_cluster(&carr[idx]);
	//shift cluster in array from index idx to left 
	for (int i=idx;i<new_narr; i++) {
		carr[i] = carr[i+1];
	}

	return new_narr;

}



/* Pocita Euklidovskou vzdalenost mezi dvema objekty*/

float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
	assert(o1);
	assert(o2);

	//calculating Euclid distance between objects o1 and o2 
	float result;
	float pow2_distance_x = (((o1->x) - (o2->x)) * ((o1->x) - (o2->x)));
	float pow2_distance_y = (((o1->y) - (o2->y)) * ((o1->y) - (o2->y)));
	result = sqrtf(pow2_distance_x + pow2_distance_y);
  	return result;

}



// Counting distance 2 clusters 

float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
	assert(c1);
	assert(c1->size > 0);
	assert(c1->obj);
	assert(c2);
	assert(c2->size > 0);
	assert(c2->obj);

	float max_distance = 1000.0 ;
	float distance;
	//Counting distance all objects in cluster c1 with all objects in cluster c2 
	for(int i=0; i < c1->size; i++) {
		for(int j=0; j < c2->size; j++) {
			//calculating the cluster 
			distance = obj_distance(&c1->obj[i], &c2->obj[j]);
			//searching max distance 
			if (distance < max_distance) {
				max_distance = distance;
			}
		}
	}
	
	return max_distance;
}



/* Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
	assert(carr);
	assert(narr > 0);
	assert(c1);
	assert(c2);
	//v poli je pouze jeden zhluk
	if (narr == 1) {
		*c1 = *c2 = 0;
		return;
	}

	float min_distance = -1;
	float distance;
	
	//calculating distance between all clusters
	for (int i = 0; i < narr; i++) {
		for (int j = i + 1; j < narr; j++) {
			//vypocet vzdalenosti
			distance = cluster_distance(&carr[i], &carr[j]);
			//hledani minimalni vzdalenosti (pokud je min_distance == -1, znamena to, ze este neni nastaveno)
			if(min_distance == -1 || distance < min_distance) {
				min_distance = distance;
				*c1 = i;
				*c2 = j;
			}
		}
	}
}



/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
/*
 Tisk shluku 'c' na stdout.
*//*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr) {

	assert(arr);
	*arr=NULL;
	
	// Open file on read
	FILE *file = fopen(filename,"r");
	if (!file) {
		printf("Error with opening file \n");
		return 1;
	}

	//Calculating, how many numbers is in 'n' (number)
	//I wanna count numbers in number 'n'

	int abacus = 0;
	int n = INT_MAX;
	
	while (abs(n) != 0) {
		n /= 10;
		abacus++;
	}

	//Calculating maximal size sentence
	int line_len = abacus + 12;
	char line[line_len];
	
	int line_number = 0;
	int count_load_object = 0;
	int obj_x;
	int obj_y;
	int obj_id;

	struct obj_t obj;
	struct cluster_t *cluster;
	char endchar;

	//Reading file on the lines;
	while (fgets(line, line_len, file) && ++line_number) {
		// First line, load count object in fiel
		if (line_number == 1) {
			//last character must be \n or \0
			if (sscanf(line, "count=%d%[^\n]", &count_load_object, &endchar) != 1) {
				printf("Wrong file format in line %d : ", line_number);
				fclose(file);
				return 1;
			}
			//Checking value objekt
			if (count_load_object <= 0) {
				printf("Value count of objects |(%d)| have to be more then 0", count_load_object);
				fclose(file);
				return 1;
			}


			assert(count_load_object >= 0);

			//alocation memory for array cluster
			if( ! (*arr = malloc(count_load_object*sizeof(struct cluster_t)))) {
					return 1;
			}

			//initialization array object
				for(int i = 0; i < count_load_object; i++) {
						init_cluster(&(*arr)[i],0);
				}
			
			if (! *arr) {
					printf("Wrong alocation memory");
					fclose(file);
					return 1;
			}
	
			continue; 
		}

		//Loading objects

		//If count of objects is more then given count, so others setences with objects ignore between each other
		if (line_number - 1 > count_load_object) {
			break;
		}

		//Load objects, checking coordinates 
		//Control ID in file
		//Behind value second coordinate cant be next characters except for \n \0
		if (
			sscanf(line, "%d %d %d%[^\n]", &obj_id, &obj_x, &obj_y, &endchar) != 3 ||
			obj_x > 1000 ||
			obj_x < 0 ||
			obj_y > 1000 ||
			obj_y < 0 
		   ) {
			printf("Wrong format file in line %d", line_number);
		

			//clear memory for object array 
			for (int i=0; i<count_load_object; i++) {	
				clear_cluster(arr[i]);
			}
			//make memory for array cluster 
			free(arr);

			*arr = NULL;
			fclose(file);
			return 1;
		}

		//Assign loaded values to structures 'obj_t'
		obj.x = obj_x;
		obj.y = obj_y;
		obj.id = obj_id;

		//Adding object to specific cluster (cluster with index 'line_number -2', because objects starting to second line and indexes have starting number 0
		for(int i=0; i < (line_number - 1); i++ ) {
			cluster = &(*arr)[i];
		}
		append_cluster(cluster,obj);
		if (cluster->size != 1) {
			printf("Wrong alocation memory");
			
			//clear memory for object array 
			for (int i=0; i<count_load_object; i++) {	
				clear_cluster(arr[i]);
			}
			//make memory for array cluster 
			free(arr);

			*arr = NULL;		
			fclose(file);
			return 1;

		}
	}

	fclose(file);

	//Count of objects in file cant be less then given count
	if (line_number - 1 < count_load_object) {
		printf("Count of objects in file : %d have to be more or equal value %d", line_number-1, count_load_object);
		//clear memory for object array 
			for (int i=0; i < count_load_object; i++) {	
				clear_cluster(arr[i]);
			}
			//make memory for array cluster 
			free(arr);
		*arr = NULL;
		return 1;
	}

	return count_load_object;
}



/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr) {
    
    printf("Clusters \n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}



bool input_arguments(int argc, char *argv[]) {

	//Arguments
	if (argc > 1 && argc <= 3) {
		struct cluster_t *clusters;
		int size;
		int required_size;
 
		assert(clusters);
		//assert(required_size >= 0);

		//loading count clusters from value of argument 
		if(argc == 3) {

			char *endptr = NULL;
			required_size = (int)strtol(argv[2], &endptr, 10);
			//validace hodnoty argumentu
			if (*endptr) {
				printf("Value of argument has to be intierger %s", endptr);
				return false;
			}
			else if (required_size <= 0) {
				printf("Value of argument n has to be intieger number more then 0 | (%d) \n",required_size);
				return false;
			}

		}
		// nacitanie zhluku
		if ((size = load_clusters(argv[1], &clusters)) == -1) {
			return false;
		}
		
		//Validation count Clusters again specific Value 
		if (required_size > size) {
			printf("Zadany pocet pozadovanych zhlukov (%d) je vetsi nez celkovy pocet objektov (%d) \n", required_size, size);
			return 1;
		}
		

		int c1_idx;
		int c2_idx;
		int old_c1_size;

		//whicle count of clusters is not equal given count, neighbour clusters join between each other
		while(size > required_size) {
			find_neighbours(clusters, size, &c1_idx, &c2_idx);
			//sticking neighbour clusters to clusters on idex c1_idx
			old_c1_size = clusters[c1_idx].size;
			merge_clusters(&clusters[c1_idx], &clusters[c2_idx]);
			if (clusters[c2_idx].size > 0 && clusters[c1_idx].size != old_c1_size + clusters[c2_idx].size) {
				printf("Chyba alokace pamate");
				return 1;
			}
			//deleting cluster in array from index c2_idx 
			size = remove_cluster(clusters, size, c2_idx);
		}

		//printf cluster
		print_clusters(clusters, size);
		//clear memory for object array 
			for (int i=0; i<size; i++) {	
				clear_cluster(&clusters[i]);
			}
			//make memory for array cluster 
			free(clusters);

		return true;
	}
	return true;
}



int main(int argc, char *argv[]) {

	bool result = input_arguments(argc, argv);
	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

