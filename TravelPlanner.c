#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INF 9999
#define MAX 10

// Structure to represent a node in adjacency list
typedef struct Node {
    int city, distance;
    struct Node* next;
} Node;

// Graph structure
typedef struct Graph {
    int numCities;
    Node* adjList[MAX];
} Graph;

// Min-Heap Node structure
typedef struct MinHeapNode {
    int city, distance;
} MinHeapNode;

// Min-Heap structure
typedef struct MinHeap {
    int size, capacity;
    int pos[MAX];
    MinHeapNode* nodes[MAX];
} MinHeap;

// Create a new graph
Graph* createGraph(int numCities) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numCities = numCities;
    for (int i = 0; i < numCities; i++)
        graph->adjList[i] = NULL;
    return graph;
}

// Add edge to the graph
void addEdge(Graph* graph, int src, int dest, int distance) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->city = dest;
    newNode->distance = distance;
    newNode->next = graph->adjList[src];
    graph->adjList[src] = newNode;

    newNode = (Node*)malloc(sizeof(Node));
    newNode->city = src;
    newNode->distance = distance;
    newNode->next = graph->adjList[dest];
    graph->adjList[dest] = newNode;
}

// Create a new Min-Heap Node
MinHeapNode* createMinHeapNode(int city, int distance) {
    MinHeapNode* node = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    node->city = city;
    node->distance = distance;
    return node;
}

// Create a Min-Heap
MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    for (int i = 0; i < MAX; i++) minHeap->pos[i] = -1;
    return minHeap;
}

// Swap two Min-Heap Nodes
void swapMinHeapNodes(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* temp = *a;
    *a = *b;
    *b = temp;
}

// Heapify function
void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx, left = 2 * idx + 1, right = 2 * idx + 2;
    
    if (left < minHeap->size && minHeap->nodes[left]->distance < minHeap->nodes[smallest]->distance)
        smallest = left;
    if (right < minHeap->size && minHeap->nodes[right]->distance < minHeap->nodes[smallest]->distance)
        smallest = right;
    
    if (smallest != idx) {
        MinHeapNode* smallestNode = minHeap->nodes[smallest];
        MinHeapNode* idxNode = minHeap->nodes[idx];

        minHeap->pos[smallestNode->city] = idx;
        minHeap->pos[idxNode->city] = smallest;

        swapMinHeapNodes(&minHeap->nodes[smallest], &minHeap->nodes[idx]);
        minHeapify(minHeap, smallest);
    }
}

// Extract the minimum node from Min-Heap
MinHeapNode* extractMin(MinHeap* minHeap) {
    if (minHeap->size == 0) return NULL;

    MinHeapNode* root = minHeap->nodes[0];
    MinHeapNode* lastNode = minHeap->nodes[minHeap->size - 1];

    minHeap->nodes[0] = lastNode;
    minHeap->pos[root->city] = minHeap->size - 1;
    minHeap->pos[lastNode->city] = 0;

    --minHeap->size;
    minHeapify(minHeap, 0);
    return root;
}

// Update distance of a city in Min-Heap
void decreaseKey(MinHeap* minHeap, int city, int distance) {
    int i = minHeap->pos[city];
    minHeap->nodes[i]->distance = distance;

    while (i && minHeap->nodes[i]->distance < minHeap->nodes[(i - 1) / 2]->distance) {
        minHeap->pos[minHeap->nodes[i]->city] = (i - 1) / 2;
        minHeap->pos[minHeap->nodes[(i - 1) / 2]->city] = i;
        swapMinHeapNodes(&minHeap->nodes[i], &minHeap->nodes[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// Check if a city is in Min-Heap
int isInMinHeap(MinHeap* minHeap, int city) {
    return minHeap->pos[city] < minHeap->size;
}

// Dijkstra’s Algorithm (Using Min-Heap)
void dijkstra(Graph* graph, int src, int dest) {
    int dist[MAX], parent[MAX];
    MinHeap* minHeap = createMinHeap(graph->numCities);

    for (int i = 0; i < graph->numCities; i++) {
        dist[i] = INF;
        parent[i] = -1;
        minHeap->nodes[i] = createMinHeapNode(i, dist[i]);
        minHeap->pos[i] = i;
    }

    minHeap->nodes[src] = createMinHeapNode(src, 0);
    minHeap->pos[src] = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);
    minHeap->size = graph->numCities;

    while (minHeap->size) {
        MinHeapNode* minNode = extractMin(minHeap);
        int u = minNode->city;
        free(minNode);

        Node* temp = graph->adjList[u];
        while (temp) {
            int v = temp->city;
            if (isInMinHeap(minHeap, v) && dist[u] != INF && temp->distance + dist[u] < dist[v]) {
                dist[v] = dist[u] + temp->distance;
                parent[v] = u;
                decreaseKey(minHeap, v, dist[v]);
            }
            temp = temp->next;
        }
    }

    printf("\nShortest Distance from City %d to City %d: %d km\n", src, dest, dist[dest]);

    printf("Path: ");
    int path[MAX], index = 0, temp = dest;
    while (temp != -1) {
        path[index++] = temp;
        temp = parent[temp];
    }
    for (int i = index - 1; i >= 0; i--) {
        printf("%d ", path[i]);
        if (i > 0) printf("-> ");
    }
    printf("\n");
}

// Main function
int main() {
    int cities, routes;
    printf("Enter number of cities: ");
    scanf("%d", &cities);

    Graph* graph = createGraph(cities);

    printf("Enter number of routes: ");
    scanf("%d", &routes);

    printf("Enter routes (City1 City2 Distance):\n");
    for (int i = 0; i < routes; i++) {
        int cityA, cityB, distance;
        scanf("%d %d %d", &cityA, &cityB, &distance);
        addEdge(graph, cityA, cityB, distance);
    }

    int startCity, endCity;
    printf("Enter source city: ");
    scanf("%d", &startCity);
    printf("Enter destination city: ");
    scanf("%d", &endCity);

    dijkstra(graph, startCity, endCity);

    return 0;
}