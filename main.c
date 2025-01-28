

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BOOKS 1000
#define MAX_USERS 100
#define MAX_NAME_LENGTH 100
#define MAX_GENRE_LENGTH 50
#define MAX_AUTHOR_LENGTH 100
#define HASH_SIZE 101
#define BUFFER_SIZE 512  // Buffer size
//  book
typedef struct Book {
    int id;
    char title[MAX_NAME_LENGTH];
    char author[MAX_AUTHOR_LENGTH];
    char genre[MAX_GENRE_LENGTH];
    float rating;
    int popularity;
} Book;

//  user
typedef struct User {
    int id;
    char name[MAX_NAME_LENGTH];
    int preferredBooks[MAX_BOOKS];
    int prefCount;
} User;

//hash table node
typedef struct HashNode {
    User user;
    struct HashNode* next;
} HashNode;

// adjacency list node
typedef struct AdjListNode {
    int dest;
    struct AdjListNode* next;
} AdjListNode;

// Structure to represent a graph
typedef struct Graph {
    int numBooks;
    AdjListNode** adjLists;  // Array of adjacency lists
} Graph;

// Global arrays and variables
Book books[MAX_BOOKS];
int bookCount = 0;

HashNode* hashTable[HASH_SIZE];
int userCount = 0;

// Function
void initializeHashTable();
int hashFunction(int userId);
void insertUser(User user);
User* searchUser(int userId);
void loadBooksFromCSV(const char* filename);
void trim(char* str);
Graph* createGraph(int numBooks);
AdjListNode* createAdjListNode(int dest);
void addEdge(Graph* graph, int src, int dest);
void buildBookGraph(Graph* graph);
void displayBooks();
void addUser();
void displayUsers();
void displayPopularBooks();
void displayUnderratedBooks();
void recommendBooks(User* user, Graph* graph);
void bfs_graph(Graph* graph, int startId, int visited[], int recommendations[], int* recCount);
int compare_popularity_desc(const void* a, const void* b);
void freeGraph(Graph* graph);

// Main Function
int main() {
    int choice;
    Graph* graph = NULL;

    // Initialize hash table
    initializeHashTable();

    // Load books from CSV
    loadBooksFromCSV("books.csv");
    if(bookCount == 0){
        printf("No books loaded. Please check the CSV file.\n");
        return 1;
    }

    // Create and build the graph
    graph = createGraph(bookCount);
    buildBookGraph(graph);

    // Main Menu Loop
    while (1) {
        printf("\n--- Book Recommendation System ---\n");
        printf("1. Display All Books\n");
        printf("2. Add User\n");
        printf("3. Display Users\n");
        printf("4. Display Most Popular Books\n");
        printf("5. Display Most Underrated Books\n");
        printf("6. Recommend Books to a User\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        if(scanf("%d", &choice)!=1){
            printf("Invalid input! Please enter a number.\n");
            while(getchar()!='\n'); // Clear input buffer
            continue;
        }
        getchar(); // Consume newline

        switch (choice) {
            case 1:
                displayBooks();
                break;
            case 2:
                addUser();
                break;
            case 3:
                displayUsers();
                break;
            case 4:
                displayPopularBooks();
                break;
            case 5:
                displayUnderratedBooks();
                break;
            case 6: {
                int userId;
                printf("Enter User ID for recommendations: ");
                if(scanf("%d", &userId)!=1){
                    printf("Invalid input! Please enter a number.\n");
                    while(getchar()!='\n');
                    break;
                }
                getchar(); // Consume newline
                User* user = searchUser(userId);
                if (user == NULL) {
                    printf("User not found!\n");
                } else {
                    recommendBooks(user, graph);
                }
                break;
            }
            case 7:
                printf("Exiting...\n");
                freeGraph(graph);
                // Free hash table memory
                for(int i=0;i<HASH_SIZE;i++) {
                    HashNode* node = hashTable[i];
                    while(node != NULL) {
                        HashNode* temp = node;
                        node = node->next;
                        free(temp);
                    }
                }
                exit(0);
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }

    return 0;
}

// Function to initialize the hash table
void initializeHashTable() {
    for(int i=0;i<HASH_SIZE;i++) {
        hashTable[i] = NULL;
    }
}

// Simple hash function for user IDs
int hashFunction(int userId) {
    return userId % HASH_SIZE;
}

// Function to insert a user into the hash table
void insertUser(User user) {
    int hashIndex = hashFunction(user.id);
    HashNode* newNode = (HashNode*) malloc(sizeof(HashNode));
    if(newNode == NULL){
        printf("Memory allocation failed!\n");
        return;
    }
    newNode->user = user;
    newNode->next = hashTable[hashIndex];
    hashTable[hashIndex] = newNode;
}

// Function to search for a user in the hash table
User* searchUser(int userId) {
    int hashIndex = hashFunction(userId);
    HashNode* node = hashTable[hashIndex];
    while(node != NULL) {
        if(node->user.id == userId)
            return &(node->user);
        node = node->next;
    }
    return NULL;
}

// Function to load books from a CSV file
void loadBooksFromCSV(const char* filename) {
    FILE* file = fopen(filename, "r");
    if(file == NULL){
        printf("Could not open file %s\n", filename);
        return;
    }

    char buffer[BUFFER_SIZE];
    int line = 0;
    while(fgets(buffer, BUFFER_SIZE, file)){
        line++;
        if(line == 1){
            // Skip header
            continue;
        }
        if(bookCount >= MAX_BOOKS){
            printf("Maximum book limit reached. Some books may not be loaded.\n");
            break;
        }
        // Parse CSV line
        char* token;
        int field = 0;
        Book newBook;
        token = strtok(buffer, ",");
        while(token != NULL){
            trim(token);
            switch(field){
                case 0:
                    newBook.id = atoi(token);
                    break;
                case 1:
                    strncpy(newBook.title, token, MAX_NAME_LENGTH);
                    newBook.title[MAX_NAME_LENGTH-1] = '\0';
                    break;
                case 2:
                    strncpy(newBook.author, token, MAX_AUTHOR_LENGTH);
                    newBook.author[MAX_AUTHOR_LENGTH-1] = '\0';
                    break;
                case 3:
                    strncpy(newBook.genre, token, MAX_GENRE_LENGTH);
                    newBook.genre[MAX_GENRE_LENGTH-1] = '\0';
                    break;
                case 4:
                    newBook.rating = atof(token); // Parse rating as float
                    break;
                default:
                    break;
            }
            field++;
            token = strtok(NULL, ",");
        }
        if(field < 5){
            printf("Incomplete data at line %d. Skipping.\n", line);
            continue;
        }
        newBook.popularity = 0; // Initialize popularity
        books[bookCount++] = newBook;
    }

    fclose(file);
    printf("Loaded %d books from %s\n", bookCount, filename);
}

// Function to trim whitespace and newline characters
void trim(char* str) {
    int len = strlen(str);
    // Trim trailing newline
    if(len > 0 && str[len-1] == '\n')
        str[len-1] = '\0';
    // Trim leading spaces
    int start = 0;
    while(str[start] == ' ') start++;
    if(start > 0)
        memmove(str, str + start, len - start +1);
}

// Function to create a graph
Graph* createGraph(int numBooks) {
    Graph* graph = (Graph*) malloc(sizeof(Graph));
    if(graph == NULL){
        printf("Memory allocation failed!\n");
        exit(1);
    }
    graph->numBooks = numBooks;
    graph->adjLists = (AdjListNode**) malloc(numBooks * sizeof(AdjListNode*));
    if(graph->adjLists == NULL){
        printf("Memory allocation failed!\n");
        free(graph);
        exit(1);
    }
    for (int i = 0; i < numBooks; i++)
        graph->adjLists[i] = NULL;
    return graph;
}

// Function to create an adjacency list node
AdjListNode* createAdjListNode(int dest) {
    AdjListNode* newNode = (AdjListNode*) malloc(sizeof(AdjListNode));
    if(newNode == NULL){
        printf("Memory allocation failed!\n");
        exit(1);
    }
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}

// Function to add an edge to the graph (undirected)
void addEdge(Graph* graph, int src, int dest) {
    AdjListNode* newNode = createAdjListNode(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;

    // Since the graph is undirected, add an edge from dest to src as well
    newNode = createAdjListNode(src);
    newNode->next = graph->adjLists[dest];
    graph->adjLists[dest] = newNode;
}

// Function to build the graph based on shared authors or genres
void buildBookGraph(Graph* graph) {
    for(int i=0;i<graph->numBooks;i++) {
        for(int j=i+1;j<graph->numBooks;j++) {
            // Connect books with the same author or same genre
            if(strcmp(books[i].author, books[j].author) == 0 || strcmp(books[i].genre, books[j].genre) == 0){
                addEdge(graph, i, j);
            }
        }
    }
    printf("Book graph built based on shared authors and genres.\n");
}

// Function to display all books
void displayBooks() {
    if(bookCount == 0) {
        printf("No books to display.\n");
        return;
    }
    printf("\n--- All Books ---\n");
    printf("%-5s %-40s %-25s %-15s %-7s %-10s\n", "ID", "Title", "Author", "Genre", "Rating", "Popularity");
    printf("--------------------------------------------------------------------------------------------------------------\n");
    for(int i=0;i<bookCount;i++) {
        printf("%-5d %-40s %-25s %-15s %-7.1f %-10d\n",
                books[i].id,
                books[i].title,
                books[i].author,
                books[i].genre,
                books[i].rating,
                books[i].popularity);
    }
    printf("--------------------------------------------------------------------------------------------------------------\n");
}

// Function to add a new user
void addUser() {
    if(userCount >= MAX_USERS){
        printf("User limit reached! Cannot add more users.\n");
        return;
    }
    User newUser;
    printf("Enter User ID (integer): ");
    if(scanf("%d", &newUser.id)!=1){
        printf("Invalid input! User not added.\n");
        while(getchar()!='\n');
        return;
    }
    getchar(); // Consume newline
    printf("Enter User Name: ");
    fgets(newUser.name, MAX_NAME_LENGTH, stdin);
    trim(newUser.name);
    newUser.prefCount = 0;
    memset(newUser.preferredBooks, -1, sizeof(newUser.preferredBooks));

    // Adding preferences
    printf("Do you want to add preferred books for %s? (1: Yes, 0: No): ", newUser.name);
    int choice;
    if(scanf("%d", &choice)!=1){
        printf("Invalid input! Skipping preferences.\n");
        while(getchar()!='\n');
    }
    getchar(); // Consume newline
    while(choice && newUser.prefCount < MAX_BOOKS){
        int bookId;
        printf("Enter Preferred Book ID: ");
        if(scanf("%d", &bookId)!=1){
            printf("Invalid input! Please enter a valid Book ID.\n");
            while(getchar()!='\n');
            continue;
        }
        getchar(); // Consume newline
        // Validate Book ID
        int found = 0;
        int bookIndex = -1;
        for(int i=0;i<bookCount;i++) {
            if(books[i].id == bookId){
                found = 1;
                bookIndex = i;
                break;
            }
        }
        if(!found){
            printf("Book ID %d not found! Please try again.\n", bookId);
            continue;
        }
        // Check if already preferred
        int alreadyPreferred = 0;
        for(int i=0;i<newUser.prefCount;i++) {
            if(newUser.preferredBooks[i] == bookIndex){
                alreadyPreferred = 1;
                break;
            }
        }
        if(alreadyPreferred){
            printf("Book \"%s\" is already in preferences.\n", books[bookIndex].title);
            continue;
        }
        // Add to preferences
        newUser.preferredBooks[newUser.prefCount++] = bookIndex;
        books[bookIndex].popularity += 1; // Increment popularity
        printf("Added \"%s\" to %s's preferences.\n", books[bookIndex].title, newUser.name);

        // Ask to add another preferred book
        printf("Do you want to add another preferred book? (1: Yes, 0: No): ");
        if(scanf("%d", &choice)!=1){
            printf("Invalid input! Stopping preference addition.\n");
            while(getchar()!='\n');
            break;
        }
        getchar(); // Consume newline
    }

    // Insert user into hash table
    insertUser(newUser);
    userCount++;
    printf("User added successfully!\n");
}

// Function to display all users
void displayUsers() {
    if(userCount == 0){
        printf("\nNo users to display.\n");
        return;
    }
    printf("\n--- Users ---\n");
    printf("%-5s %-25s %-40s\n", "ID", "Name", "Preferred Books");
    printf("--------------------------------------------------------------------------------------------------\n");
    for(int i=0;i<HASH_SIZE;i++) {
        HashNode* node = hashTable[i];
        while(node != NULL){
            printf("%-5d %-25s ", node->user.id, node->user.name);
            if(node->user.prefCount == 0){
                printf("None");
            }
            else{
                for(int j=0;j<node->user.prefCount;j++) {
                    printf("\"%s\" (ID: %d)", books[node->user.preferredBooks[j]].title, books[node->user.preferredBooks[j]].id);
                    if(j != node->user.prefCount -1)
                        printf(", ");
                }
            }
            printf("\n");
            node = node->next;
        }
    }
    printf("--------------------------------------------------------------------------------------------------\n");
}

// Function to display most popular books
void displayPopularBooks() {
    if(bookCount == 0) {
        printf("No books available.\n");
        return;
    }
    printf("\n--- Most Popular Books ---\n");
    printf("%-5s %-40s %-25s %-15s %-7s %-10s\n", "ID", "Title", "Author", "Genre", "Rating", "Popularity");
    printf("--------------------------------------------------------------------------------------------------------------\n");
    // Define a threshold for popularity, e.g., popularity > 5
    int threshold = 5;
    int found = 0;
    for(int i=0;i<bookCount;i++) {
        if(books[i].popularity > threshold){
            printf("%-5d %-40s %-25s %-15s %-7.1f %-10d\n",
                books[i].id,
                books[i].title,
                books[i].author,
                books[i].genre,
                books[i].rating,
                books[i].popularity);
            found = 1;
        }
    }
    if(!found){
        printf("No popular books found with popularity greater than %d.\n", threshold);
    }
    printf("--------------------------------------------------------------------------------------------------------------\n");
}

// Function to display most underrated books
void displayUnderratedBooks() {
    if(bookCount == 0) {
        printf("No books available.\n");
        return;
    }
    printf("\n--- Most Underrated Books ---\n");
    printf("%-5s %-40s %-25s %-15s %-7s %-10s\n", "ID", "Title", "Author", "Genre", "Rating", "Popularity");
    printf("--------------------------------------------------------------------------------------------------------------\n");
    // Define a threshold for underrated, e.g., popularity <= 2
    int threshold = 2;
    int found = 0;
    for(int i=0;i<bookCount;i++) {
        if(books[i].popularity <= threshold){
            printf("%-5d %-40s %-25s %-15s %-7.1f %-10d\n",
                books[i].id,
                books[i].title,
                books[i].author,
                books[i].genre,
                books[i].rating,
                books[i].popularity);
            found = 1;
        }
    }
    if(!found){
        printf("No underrated books found with popularity less than or equal to %d.\n", threshold);
    }
    printf("--------------------------------------------------------------------------------------------------------------\n");
}

// Function to recommend books to a user based on preferences and genre
void recommendBooks(User* user, Graph* graph) {
    if(user->prefCount == 0) {
        printf("User has no preferred books to base recommendations on.\n");
        return;
    }

    char desiredGenre[MAX_GENRE_LENGTH];
    printf("Enter the genre you are interested in for recommendations: ");
    fgets(desiredGenre, MAX_GENRE_LENGTH, stdin);
    trim(desiredGenre); // Remove any trailing newline or spaces

    if(strlen(desiredGenre) == 0) {
        printf("No genre entered. Proceeding without genre filtering.\n");
    }

    // Validate the entered genre
    int genreExists = 0;
    for(int i=0;i<bookCount;i++) {
        if(strcmp(books[i].genre, desiredGenre) == 0) {
            genreExists = 1;
            break;
        }
    }
    if(!genreExists) {
        printf("Genre \"%s\" not found in the library. Please check the genre and try again.\n", desiredGenre);
        return;
    }

    // Ask user preference: popular or underrated
    int popChoice;
    printf("Do you want (1) Popular or (2) Underrated book recommendations? Enter 1 or 2: ");
    if(scanf("%d", &popChoice)!=1){
        printf("Invalid input! Proceeding with popular recommendations by default.\n");
        popChoice = 1;
        while(getchar()!='\n');
    }
    getchar(); // Consume newline

    if(popChoice !=1 && popChoice !=2){
        printf("Invalid choice! Proceeding with popular recommendations by default.\n");
        popChoice = 1;
    }

    int desiredPopularity; // 1 for popular, 0 for underrated
    if(popChoice ==1)
        desiredPopularity = 1;
    else
        desiredPopularity = 0;

    int visited[MAX_BOOKS];
    for(int i=0;i<bookCount;i++) visited[i] = 0;

    int recommendations[MAX_BOOKS];
    int recCount = 0;

    // Traverse graph for each preferred book
    for(int i=0;i<user->prefCount;i++) {
        int prefIndex = user->preferredBooks[i];
        bfs_graph(graph, prefIndex, visited, recommendations, &recCount);
    }

    // Remove duplicates and already preferred books
    int uniqueRecommendations[MAX_BOOKS];
    int uniqueCount = 0;
    for(int i=0;i<recCount;i++) {
        int alreadyPreferred = 0;
        for(int j=0;j<user->prefCount;j++) {
            if(recommendations[i] == user->preferredBooks[j]) {
                alreadyPreferred = 1;
                break;
            }
        }
        if(!alreadyPreferred) {
            // Check for uniqueness
            int isUnique = 1;
            for(int k=0;k<uniqueCount;k++) {
                if(uniqueRecommendations[k] == recommendations[i]) {
                    isUnique = 0;
                    break;
                }
            }
            if(isUnique) {
                uniqueRecommendations[uniqueCount++] = recommendations[i];
            }
        }
    }

    if(uniqueCount == 0) {
        printf("No recommendations available based on current preferences.\n");
        return;
    }

    // Filter recommendations based on desired genre
    int genreFilteredRecommendations[MAX_BOOKS];
    int genreCount = 0;
    for(int i=0;i<uniqueCount;i++) {
        if(strcmp(books[uniqueRecommendations[i]].genre, desiredGenre) == 0) {
            genreFilteredRecommendations[genreCount++] = uniqueRecommendations[i];
        }
    }

    if(genreCount == 0) {
        printf("No recommendations found in the genre \"%s\" based on your preferences.\n", desiredGenre);
        return;
    }

    // Further filter based on popularity preference
    int finalRecommendations[MAX_BOOKS];
    int finalCount = 0;
    for(int i=0;i<genreCount;i++) {
        if(desiredPopularity) { // Popular
            if(books[genreFilteredRecommendations[i]].popularity > 5)
                finalRecommendations[finalCount++] = genreFilteredRecommendations[i];
        }
        else { // Underrated
            if(books[genreFilteredRecommendations[i]].popularity <= 2) // Example threshold
                finalRecommendations[finalCount++] = genreFilteredRecommendations[i];
        }
    }

    if(finalCount == 0) {
        if(desiredPopularity)
            printf("No popular recommendations found in the genre \"%s\" based on your preferences.\n", desiredGenre);
        else
            printf("No underrated recommendations found in the genre \"%s\" based on your preferences.\n", desiredGenre);
        return;
    }

    // Sort final recommendations by popularity descending
    qsort(finalRecommendations, finalCount, sizeof(int), compare_popularity_desc);

    // Display recommendations
    printf("\n--- Recommendations for %s ---\n", user->name);
    printf("%-5s %-40s %-25s %-15s %-7s %-10s\n", "ID", "Title", "Author", "Genre", "Rating", "Popularity");
    printf("--------------------------------------------------------------------------------------------------------------\n");
    for(int i=0;i<finalCount && i < 10;i++) { // Limit to top 10 recommendations
        printf("%-5d %-40s %-25s %-15s %-7.1f %-10d\n",
            books[finalRecommendations[i]].id,
            books[finalRecommendations[i]].title,
            books[finalRecommendations[i]].author,
            books[finalRecommendations[i]].genre,
            books[finalRecommendations[i]].rating,
            books[finalRecommendations[i]].popularity);
    }
    printf("--------------------------------------------------------------------------------------------------------------\n");
}

// BFS function to traverse the graph and collect related books
void bfs_graph(Graph* graph, int startId, int visited[], int recommendations[], int* recCount) {
    int queue[MAX_BOOKS];
    int front = 0, rear = 0;
    queue[rear++] = startId;
    visited[startId] = 1;

    while(front < rear) {
        int current = queue[front++];
        AdjListNode* temp = graph->adjLists[current];
        while(temp != NULL) {
            int adj = temp->dest;
            if(!visited[adj]) {
                visited[adj] = 1;
                queue[rear++] = adj;
                recommendations[(*recCount)++] = adj;
            }
            temp = temp->next;
        }
    }
}

// Comparator function for qsort (descending order of popularity)
int compare_popularity_desc(const void* a, const void* b) {
    int bookA = *(int*)a;
    int bookB = *(int*)b;
    return books[bookB].popularity - books[bookA].popularity;
}

// Function to free the graph's adjacency lists
void freeGraph(Graph* graph) {
    for(int i=0;i<graph->numBooks;i++) {
        AdjListNode* node = graph->adjLists[i];
        while(node != NULL){
            AdjListNode* temp = node;
            node = node->next;
            free(temp);
        }
    }
    free(graph->adjLists);
    free(graph);
}
