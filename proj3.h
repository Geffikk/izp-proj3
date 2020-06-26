/**
	* @mainpage Projekt 3
	* @link
	* proj3.h
	* @endling

	* @file proj3.h
	* @brief Projekt 3 - Jednoducha shlukova analyza (Complete linkage)
	* @autor Maros Geffert <xgeffe00@stud.fit.vutbr.cz>
	* @date Decemeber 2018
	* @version 1.0
*/



/** Struktura znazornujuca objekt s ID a nejakymi suradnicami (x,y) */
struct obj_t {
    /** identifikator */
    int id;
    /** suradnica x */
    float x;
    /** suradnica y */
    float y;
};

/** Struktura znazornujuca zhluk objketov */
struct cluster_t {
    /** Pocet objektov vo zhluku */
    int size;
    /** kapacita zhluku, resp. rezervujem miesto pre urcity pocet objektov */
    int capacity;
    /** Pole objektov patriaci danemu zhluku */
    struct obj_t *obj;
};



/**
 * @defgroup clusters Praca so zhlukmi
 * @{
 */

/**
	* Inicializace shluku `c`, alokuje pamet pro `cap` objektu.
 	* Ukazatel NULL u pole objektu znamena kapacitu 0.
 	* @post
 	* Zhluk 'c' bude mat alokovanu pamat pre 'cap' objektu pomocou funkcie malloc
 	* Pokial sa do zhluku nic neprida resp. (c->obj == NULL) nastane chyba alokacie
 	* 
 	* @param c zhluk pre inicializaciu
 	* @param cap, kapacita zhluku
*/ 

void init_cluster(struct cluster_t *c, int cap);



/**
	* Odstranenie vsetky objektov v zhluku 'c' a incializacia na prazny zluk
	*
	* @post 'c' alokovana pamat pre objekty sa uvolni
	*
	* @param zluk 'c' pre odstranenie
*/

void clear_cluster(struct cluster_t *c);



/* Hodnota pre realokaciu zhluku */
extern const int CLUSTER_CHUNK;



/**
	* Zmena kapacity zhluku 'c'
	*
	* @pre
	* Kapacita zhluku 'c' bude vacsia alebo rovna 0
	*
	* @post
	* Kapacita zhluku 'c' s velkostou 'cap' sa zmeni na novu velkost s velkostou 'new_cap'
	* pokial 'cap' < 'new_cap' inak ostava kapacita nezmenena
	* a pokial sa kapacitu nepodari alokovat resp. (arr == NULL) tak nastane chyba alokacie
	*
	* @param 'c' zhluk pre rozsirenie kapacity
	* @param 'new_cap' nova kapacita zhluku
	*
	* @return zhluk s novou kapacitou new_cap
*/

struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);



/**
	* Prida objekt 'obj' na koniec zhluku 'c'. Rozsiri zhluk, pokial se do nej objekt nevojde.
	*
	* @pre
	* Pocet objektov v zhluku sa zmeni, resp. bude vacsi alebo rovny 0
	*
	* @post
	* Na koniec zhluku 'c' bude pridany objekt 'obj' a zhluk sa rozsiri
	* Pokial nenastane chyba pri alokacii
	*
	* @param 'c' zhluk do ktoreho na prvu volnu poziciu pridavam 'obj'
	* ak nie je volna pozicia zhluk rozsirujem a pridavam 'obj'
	* @param 'obj' objekt, ktory pridavam do zhluku
*/

void append_cluster(struct cluster_t *c, struct obj_t obj);



/**
	* Do zhluku 'c1' prida objekty zhluku 'c2'. Zhluk 'c1' bude v pripade nutnosti rozsireny
	* Objekty v zhluku 'c1' budu zoradane vzostupne podla ID. Zhluk 'c2' bude nezmeneny
	*
	* @pre
	* Pocet objektov v zhluku 'c2' bude vacsi alebo rovny 0
	*
	* @post
	* Zhluk 'c1' bude v pripade nutnosti rozsireny a pridaju sa donho objekty zhluku 'c2'
	* Pokial nenastane chyba pri alokacii
	* Objekty v zhluku 'c1' budu zoradene vzostupne podla ID
	*
	* @param zhluk 'c1', do ktoreho su pridane objekty zo zhluku 'c2'
	* a ktory sa zoradi vzostupne podla ID
	* @param zhluk 'c2', ktoreho objekty su pridane do zhluku 'c1'
*/

void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);



/**
 * @}
 */


/**
 * @defgroup array_of_clusters Praca s polom zhluku
 * @{
 */

/**
	* Odstrani zhluk z pola zhlukou 'carr'
	*
	* @post
	* Z pola zhluku 'carr' sa odstrani prvok na indexe 'idx' a pole sa zmensi o 1
	*
	* @param 'carr' pole zhlukou
	* @param 'narr' pocet zhlukou v poli 'carr'
	* @param 'idx' index zhluku pre odstranenie
	*
	* @return novy pocet zhlukou v poli 'carr'
*/

int remove_cluster(struct cluster_t *carr, int narr, int idx);



/**
	* Pocita Euklidovsku vzdialenost medzi dvoma objektami
	*
	* @param 'o1' objekt 1
	* @param 'o2' objekt 2
	*
	* @return Euklidovsku vzdialenost medzi objektami 'o1' a 'o2'
*/

float obj_distance(struct obj_t *o1, struct obj_t *o2);



/**
	* Pocita vzdialenost dvoch zhlukou
	*
	* @pre
	* Pocet objektov v zhluku 'c1' bude vacsi ako 0
	* Pocet objektov v zhluku 'c2' bude vacsi ako 0
	*
	* @param 'c1' zhluk 1
	* @param 'c2' zhluk 2
	*
	* @return vzdialenost medzi zhlukmi 'c1' a 'c2'
*/

float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);



/**
	* Najde dvoch najblizsich susedov
	* Indexy najdenych zhlukou v poli 'carr' sa ulozia do zhlukou 'c1' a 'c2'
	*
	* @post
	* Indexy dvoch najblizsich susedov budu ulozene do zhlukou 'c1' a 'c2'
	*
	* @param 'carr' pole zhlukou
	* @param 'narr' pocet zhlukou v poli
	* @param 'c1' index prveho najdeneho zhluku v poli
	* @param 'c2' index druheho najdeneho zhluku v poli
*/

void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);



/**
	* Sortovanie objektov v zhluku podla ID (vzostupne)
	*
	* @post
	* Objekty v zhluku 'c' budu zoradene vzostupne podla ID
	*
	* @param zhluk 'c', zhluk na zoradenie
*/

void sort_cluster(struct cluster_t *c);



/**
	* Tlac zhluku na stdout
	*
	* @post
	* Objekty zhluku sa vypisu na standartny vystup (stdout)
	*
	* @param 'c' zhluk pre tlac
*/

void print_cluster(struct cluster_t *c);



/**
	* Zo suboru 'filename' nacitavam objekty.
	* Pre kazdy objekt vytvorim zhluk a ulozim ho do pola zhluku
	* Alokujem pamat v poli pre vsetky zhluky a ukazatel na prvu polozku pola alokuje do pamati
	* kam sa odkazuje parameter 'arr'
	* (Ukazatel na prvy zhluk v alokovanom poli)
	*
	* @pre
	* Funkcia precita subor 'filename' a bude mat prava na jeho precitanie
	* Data v subore budu v spravnom formate
	*
	* @post
	* Pro kazdy objekt uvedeny ve vstupnim souboru bude vytvoren shluk,
	* vsechny tyto shluky budou ulozeny v poli shluku `arr`, ktere bude
	* mit alokovanou pamet pro pocet shluku uvedeny ve vstupnim souboru,
	* pokud nenastane chyba pri alokaci.
	*
	* @param filename nazev souboru pro nacteni objektu
	* @param arr ukazatel na pole shluku nactenych ze souboru (v pripade chyby bude ukazovat na NULL)
	* @return pocet nactenych shluku (v pripade chyby -1)
	*/

int load_clusters(char *filename, struct cluster_t **arr);



/**
	* Tlac pola zhlukou na stdout
	*
	* @post
	* Objekty vsetkych zhlukou budu vytlacene na standartny vystup (stdout)
	*
	* @param 'carr' pole zhlukou
	* @param 'narr' pocet zhlukou v poli
*/

void print_clusters(struct cluster_t *carr, int narr);
