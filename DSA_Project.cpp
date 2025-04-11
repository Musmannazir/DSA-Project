#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>
const int infinity = INT_MAX;
using namespace std;

/*
Pizza Shop Ordering Management System
GitHub Repository: https://github.com/[your-username]/pizza-ordering-system

Features:
- Three customer types: Take-Away, Dine-In, Home Delivery
- Priority-based serving for different customer types
- AVL Tree for efficient customer record keeping
- Dijkstra's algorithm for delivery route optimization
- Comprehensive order management and reporting
*/

// Defining Data Types
struct customer {
    int age;
    string name;
    string pizzaName;
    int quantity;
    double bill;

    customer() {}

    customer(int age, string name, int quantity, string pizzaName, double bill) {
        this->age = age;
        this->name = name;
        this->pizzaName = pizzaName;
        this->quantity = quantity;
        this->bill = bill;
    }
};

struct takeAwayCustomer {
    customer cusotomer;
    takeAwayCustomer* next = NULL;

    takeAwayCustomer(int age, string name, int quantity, string pizzaName, double bill) {
        this->cusotomer = customer(age, name, quantity, pizzaName, bill);
    }
};

struct dineInCustomer {
    customer cusotomer;
    dineInCustomer* next = NULL;

    dineInCustomer(int age, string name, int quantity, string pizzaName, double bill) {
        this->cusotomer = customer(age, name, quantity, pizzaName, bill);
    }
};

struct homeDeliveryCustomer {
    customer cusotomer;
    string address;
    double deliveryCharges;
    int distanceDelivery;
    struct homeDeliveryCustomer* next = NULL;

    homeDeliveryCustomer(int age, string name, int quantity, string pizzaName, double bill, 
                        string address, double deliveryCharges, int distanceDelivery) {
        this->cusotomer = customer(age, name, quantity, pizzaName, bill);
        this->address = address;
        this->deliveryCharges = deliveryCharges;
        this->distanceDelivery = distanceDelivery;
    }
};

struct PizzaShop {
    string shopName;
    string* menu;
    int* price;
    string address;
    takeAwayCustomer* nextTakeAwayCustomer = NULL;
    dineInCustomer* nextDineInCustomer = NULL;
    homeDeliveryCustomer* nextHomeDeliveryCustomer = NULL;
};

// Global pointers and variables
PizzaShop* myPizzaShop = NULL;
takeAwayCustomer* currentTakeAwayCustomer = NULL;
dineInCustomer* currentDineInCustomer = NULL;
homeDeliveryCustomer* currentHomeDeliveryCustomer = NULL;

double total = 0, takeAway = 0, dineIn = 0, homeDelivery = 0;
double servedTotal = 0;

// AVL Tree for served customers
struct servedCustomer {
    int age;
    string name;
    int quantity;
    string pizzaName;
    double bill;
    string customerType;
    servedCustomer* left;
    servedCustomer* right;
    int height;

    servedCustomer(int age, string name, int quantity, string pizzaName, 
                  double bill, string customerType) {
        this->age = age;
        this->name = name;
        this->quantity = quantity;
        this->pizzaName = pizzaName;
        this->bill = bill;
        this->customerType = customerType;
        this->left = NULL;
        this->right = NULL;
        this->height = 1;
    }
};

servedCustomer* root = NULL;

// AVL Tree Helper Functions
int height(servedCustomer* node) {
    if (node == NULL) return 0;
    return node->height;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int getBalance(servedCustomer* node) {
    if (node == NULL) return 0;
    return height(node->left) - height(node->right);
}

servedCustomer* rightRotate(servedCustomer* y) {
    servedCustomer* x = y->left;
    servedCustomer* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

servedCustomer* leftRotate(servedCustomer* x) {
    servedCustomer* y = x->right;
    servedCustomer* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

servedCustomer* insert(servedCustomer* node, int age, string name, int quantity, 
                       string pizzaName, double bill, string customerType) {
    if (node == NULL) {
        return new servedCustomer(age, name, quantity, pizzaName, bill, customerType);
    }

    if (name < node->name) {
        node->left = insert(node->left, age, name, quantity, pizzaName, bill, customerType);
    } else if (name > node->name) {
        node->right = insert(node->right, age, name, quantity, pizzaName, bill, customerType);
    } else {
        return node; // Duplicate names not allowed
    }

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);

    // Left Left Case
    if (balance > 1 && name < node->left->name) {
        return rightRotate(node);
    }

    // Right Right Case
    if (balance < -1 && name > node->right->name) {
        return leftRotate(node);
    }

    // Left Right Case
    if (balance > 1 && name > node->left->name) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && name < node->right->name) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

servedCustomer* minValueNode(servedCustomer* node) {
    servedCustomer* current = node;
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

servedCustomer* deleteNode(servedCustomer* root, string name) {
    if (root == NULL) return root;

    if (name < root->name) {
        root->left = deleteNode(root->left, name);
    } else if (name > root->name) {
        root->right = deleteNode(root->right, name);
    } else {
        if ((root->left == NULL) || (root->right == NULL)) {
            servedCustomer* temp = root->left ? root->left : root->right;

            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else {
                *root = *temp;
            }
            delete temp;
        } else {
            servedCustomer* temp = minValueNode(root->right);
            root->name = temp->name;
            root->right = deleteNode(root->right, temp->name);
        }
    }

    if (root == NULL) return root;

    root->height = 1 + max(height(root->left), height(root->right));
    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0) {
        return rightRotate(root);
    }

    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && getBalance(root->right) <= 0) {
        return leftRotate(root);
    }

    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

void displayCustomer(servedCustomer* node) {
    if (node != NULL) {
        cout << "-----------------------------------------------------" << endl;
        cout << "Name: " << node->name << endl;
        cout << "Age: " << node->age << endl;
        cout << "Pizza: " << node->pizzaName << endl;
        cout << "Quantity: " << node->quantity << endl;
        cout << "Bill: " << node->bill << " RS/_" << endl;
        cout << "Customer Type: " << node->customerType << endl;
        cout << "-----------------------------------------------------" << endl;
    }
}

void inOrder(servedCustomer* root) {
    if (root != NULL) {
        inOrder(root->left);
        displayCustomer(root);
        inOrder(root->right);
    }
}

servedCustomer* search(servedCustomer* root, string name) {
    if (root == NULL || root->name == name) {
        return root;
    }

    if (root->name < name) {
        return search(root->right, name);
    }

    return search(root->left, name);
}

void deleteAllServedCustomers(servedCustomer* root) {
    if (root == NULL) return;
    deleteAllServedCustomers(root->left);
    deleteAllServedCustomers(root->right);
    delete root;
}

// Order Management Functions
void placeOrderTakeAwayCustomer(int age, string name, string pizzaName, int quantity, double bill) {
    currentTakeAwayCustomer = new takeAwayCustomer(age, name, quantity, pizzaName, bill);

    if (myPizzaShop->nextTakeAwayCustomer == NULL) {
        myPizzaShop->nextTakeAwayCustomer = currentTakeAwayCustomer;
    } else {
        takeAwayCustomer* temp = myPizzaShop->nextTakeAwayCustomer;
        while (temp->next != NULL) {
            temp = temp->next;
        }

        if (temp->cusotomer.age < currentTakeAwayCustomer->cusotomer.age) {
            takeAwayCustomer* firstCustomer = myPizzaShop->nextTakeAwayCustomer;
            myPizzaShop->nextTakeAwayCustomer = currentTakeAwayCustomer;
            currentTakeAwayCustomer->next = firstCustomer;
        } else {
            temp->next = currentTakeAwayCustomer;
            currentTakeAwayCustomer->next = NULL;
        }
    }
    cout << "Your Order has been Placed MR/MRS " << name << endl;
    cout << "Order Details: " << pizzaName << " x " << quantity << endl;
    cout << "Total Bill: " << bill << " RS" << endl;
}

void serveOrderTakeAwayCustomer() {
    if (myPizzaShop->nextTakeAwayCustomer == NULL) {
        cout << "No Take Away Customer to Serve" << endl;
        return;
    }

    takeAwayCustomer* temp = myPizzaShop->nextTakeAwayCustomer;
    myPizzaShop->nextTakeAwayCustomer = temp->next;

    cout << "Take Away Customer Served: " << temp->cusotomer.name << endl;
    string customerType = "Take-Away";
    root = insert(root, temp->cusotomer.age, temp->cusotomer.name, 
                 temp->cusotomer.quantity, temp->cusotomer.pizzaName, 
                 temp->cusotomer.bill, customerType);

    delete temp;
}

void placeOrderDineInCustomer(int age, string name, string pizzaName, int quantity, double bill) {
    currentDineInCustomer = new dineInCustomer(age, name, quantity, pizzaName, bill);

    if (myPizzaShop->nextDineInCustomer == NULL) {
        myPizzaShop->nextDineInCustomer = currentDineInCustomer;
    } else {
        dineInCustomer* temp = myPizzaShop->nextDineInCustomer;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = currentDineInCustomer;
    }
    cout << "Your Order has been Placed MR/MRS " << name << endl;
    cout << "Order Details: " << pizzaName << " x " << quantity << endl;
    cout << "Total Bill: " << bill << " RS" << endl;
}

void serveOrderDineInCustomer() {
    if (myPizzaShop->nextDineInCustomer == NULL) {
        cout << "No Dine-In Customer to Serve" << endl;
        return;
    }

    dineInCustomer* temp = myPizzaShop->nextDineInCustomer;
    myPizzaShop->nextDineInCustomer = temp->next;

    cout << "Dine-In Customer Served: " << temp->cusotomer.name << endl;
    string customerType = "Dine-In";
    root = insert(root, temp->cusotomer.age, temp->cusotomer.name, 
                 temp->cusotomer.quantity, temp->cusotomer.pizzaName, 
                 temp->cusotomer.bill, customerType);

    delete temp;
}

void placeOrderHomeDeliveryCustomer(int age, string name, string pizzaName, 
                                  int quantity, double bill, string address, 
                                  int deliveryCharges, int distanceDelivery) {
    currentHomeDeliveryCustomer = new homeDeliveryCustomer(age, name, quantity, 
                                                         pizzaName, bill, address, 
                                                         deliveryCharges, distanceDelivery);

    if (myPizzaShop->nextHomeDeliveryCustomer == NULL) {
        myPizzaShop->nextHomeDeliveryCustomer = currentHomeDeliveryCustomer;
    } else {
        homeDeliveryCustomer* temp = myPizzaShop->nextHomeDeliveryCustomer;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = currentHomeDeliveryCustomer;
    }
    cout << "Your Order has been Placed MR/MRS " << name << endl;
    cout << "Order Details: " << pizzaName << " x " << quantity << endl;
    cout << "Delivery Address: " << address << endl;
    cout << "Delivery Charges: " << deliveryCharges << " RS" << endl;
    cout << "Total Bill: " << bill << " RS" << endl;
}

void serveOrderHomeDeliveryCustomer() {
    if (myPizzaShop->nextHomeDeliveryCustomer == NULL) {
        cout << "No Home Delivery Customer to Serve" << endl;
        return;
    }

    homeDeliveryCustomer* first = myPizzaShop->nextHomeDeliveryCustomer;
    homeDeliveryCustomer* prev = NULL;

    while (first->next != NULL) {
        prev = first;
        first = first->next;
    }

    if (prev == NULL) {
        myPizzaShop->nextHomeDeliveryCustomer = NULL;
    } else {
        prev->next = NULL;
    }

    cout << "Home Delivery Customer Served: " << first->cusotomer.name << endl;
    string customerType = "Home-Delivery";
    root = insert(root, first->cusotomer.age, first->cusotomer.name, 
                 first->cusotomer.quantity, first->cusotomer.pizzaName, 
                 first->cusotomer.bill, customerType);

    delete first;
}

void serveAllOrders() {
    while (myPizzaShop->nextTakeAwayCustomer != NULL) {
        serveOrderTakeAwayCustomer();
    }
    while (myPizzaShop->nextDineInCustomer != NULL) {
        serveOrderDineInCustomer();
    }
    while (myPizzaShop->nextHomeDeliveryCustomer != NULL) {
        serveOrderHomeDeliveryCustomer();
    }
}

// Display Functions
void displayTakeAwayCustomers() {
    if (myPizzaShop->nextTakeAwayCustomer == NULL) {
        cout << "No Take-Away Orders" << endl;
        return;
    }

    takeAwayCustomer* current = myPizzaShop->nextTakeAwayCustomer;
    while (current != NULL) {
        cout << "-----------------------------------------------------" << endl;
        cout << "Name: " << current->cusotomer.name << endl;
        cout << "Age: " << current->cusotomer.age << endl;
        cout << "Pizza: " << current->cusotomer.pizzaName << endl;
        cout << "Quantity: " << current->cusotomer.quantity << endl;
        cout << "Bill: " << current->cusotomer.bill << " RS/_" << endl;
        cout << "-----------------------------------------------------" << endl;
        current = current->next;
    }
}

void displayDineInCustomers() {
    if (myPizzaShop->nextDineInCustomer == NULL) {
        cout << "No Dine-In Orders" << endl;
        return;
    }

    dineInCustomer* current = myPizzaShop->nextDineInCustomer;
    while (current != NULL) {
        cout << "-----------------------------------------------------" << endl;
        cout << "Name: " << current->cusotomer.name << endl;
        cout << "Age: " << current->cusotomer.age << endl;
        cout << "Pizza: " << current->cusotomer.pizzaName << endl;
        cout << "Quantity: " << current->cusotomer.quantity << endl;
        cout << "Bill: " << current->cusotomer.bill << " RS/_" << endl;
        cout << "-----------------------------------------------------" << endl;
        current = current->next;
    }
}

void displayHomeDeliveryCustomers() {
    if (myPizzaShop->nextHomeDeliveryCustomer == NULL) {
        cout << "No Home Delivery Orders" << endl;
        return;
    }

    homeDeliveryCustomer* current = myPizzaShop->nextHomeDeliveryCustomer;
    while (current != NULL) {
        cout << "-----------------------------------------------------" << endl;
        cout << "Name: " << current->cusotomer.name << endl;
        cout << "Age: " << current->cusotomer.age << endl;
        cout << "Pizza: " << current->cusotomer.pizzaName << endl;
        cout << "Quantity: " << current->cusotomer.quantity << endl;
        cout << "Delivery Distance: " << current->distanceDelivery << " KM" << endl;
        cout << "Delivery Charges: " << current->deliveryCharges << " RS" << endl;
        cout << "Bill: " << current->cusotomer.bill << " RS/_" << endl;
        cout << "Address: " << current->address << endl;
        cout << "-----------------------------------------------------" << endl;
        current = current->next;
    }
}

void displayAllOrders() {
    cout << "\n=== Take-Away Orders ===" << endl;
    displayTakeAwayCustomers();
    
    cout << "\n=== Dine-In Orders ===" << endl;
    displayDineInCustomers();
    
    cout << "\n=== Home Delivery Orders ===" << endl;
    displayHomeDeliveryCustomers();
}

void calculatePendingBills() {
    takeAway = 0, dineIn = 0, homeDelivery = 0, total = 0;
    
    takeAwayCustomer* p = myPizzaShop->nextTakeAwayCustomer;
    while (p != NULL) {
        takeAway += p->cusotomer.bill;
        p = p->next;
    }
    
    dineInCustomer* q = myPizzaShop->nextDineInCustomer;
    while (q != NULL) {
        dineIn += q->cusotomer.bill;
        q = q->next;
    }
    
    homeDeliveryCustomer* r = myPizzaShop->nextHomeDeliveryCustomer;
    while (r != NULL) {
        homeDelivery += r->cusotomer.bill;
        r = r->next;
    }
    
    total = takeAway + dineIn + homeDelivery;
    
    cout << "Pending Bills Summary:" << endl;
    cout << "Take-Away: " << takeAway << " RS" << endl;
    cout << "Dine-In: " << dineIn << " RS" << endl;
    cout << "Home Delivery: " << homeDelivery << " RS" << endl;
    cout << "Total Pending: " << total << " RS" << endl;
}

double calculateTotalEarnings(servedCustomer* node) {
    if (node == NULL) return 0;
    return node->bill + calculateTotalEarnings(node->left) + calculateTotalEarnings(node->right);
}

// Delivery System
vector<string> deliveryPoints = {"PizzaSHOP", "Basti", "Benazir colony", 
                                "Nawab abad", "Taxila", "Model Town"};

vector<vector<pair<int, int>>> deliveryMap = {
    {{1, 2}, {2, 3}, {3, 5}, {5, 4}},
    {{0, 2}, {5, 1}},
    {{0, 3}, {3, 1}},
    {{0, 5}, {4, 2}, {5, 2}, {2, 1}},
    {{3, 2}, {5, 2}},
    {{0, 4}, {1, 1}, {3, 2}, {4, 2}}
};

vector<int> dijkstra(int source) {
    vector<int> dist(6, infinity);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    
    dist[source] = 0;
    pq.push({0, source});
    
    while (!pq.empty()) {
        int u = pq.top().second;
        int current_dist = pq.top().first;
        pq.pop();
        
        if (current_dist > dist[u]) continue;
        
        for (auto &edge : deliveryMap[u]) {
            int v = edge.first;
            int weight = edge.second;
            
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }
    
    return dist;
}

// Main Function
int main() {
    // Initialize Pizza Shop
    myPizzaShop = new PizzaShop;
    myPizzaShop->shopName = "Pizza Castle";
    myPizzaShop->address = "Officer Colony, Wahcantt";
    
    // Initialize Menu
    myPizzaShop->menu = new string[11]{
        "", "Chicken Tikka", "Arabic Ranch", "Chicken Fajita", 
        "Cheese Lover", "Chicken Supreme", "All Veggie", 
        "Garlic West", "Beef Bold", "Phantom", "Mexican Delight"
    };
    
    // Initialize Prices
    myPizzaShop->price = new int[11]{
        0, 2000, 2500, 2400, 2200, 2700, 2000, 2100, 3000, 3000, 2800
    };
    
    int choice;
    do {
        cout << "\n=== Pizza Order Management System ===" << endl;
        cout << "1. Place Take-Away Order" << endl;
        cout << "2. Place Home Delivery Order" << endl;
        cout << "3. Place Dine-In Order" << endl;
        cout << "4. Serve Take-Away Order" << endl;
        cout << "5. Serve Home Delivery Order" << endl;
        cout << "6. Serve Dine-In Order" << endl;
        cout << "7. Serve All Orders" << endl;
        cout << "8. View All Pending Orders" << endl;
        cout << "9. View Served Orders" << endl;
        cout << "10. Search Served Order" << endl;
        cout << "11. Clear Served Orders" << endl;
        cout << "12. View Pending Bills" << endl;
        cout << "13. View Total Earnings" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        
        string name, address;
        int age, quantity, pizzaChoice, deliveryChoice;
        double bill;
        
        switch (choice) {
            case 1: {
                cout << "\n=== Take-Away Order ===" << endl;
                cout << "Enter customer name: ";
                cin >> name;
                cout << "Enter customer age: ";
                cin >> age;
                
                cout << "\nMenu:" << endl;
                for (int i = 1; i <= 10; i++) {
                    cout << i << ". " << myPizzaShop->menu[i] << " - " << myPizzaShop->price[i] << " RS" << endl;
                }
                
                cout << "Select pizza (1-10): ";
                cin >> pizzaChoice;
                cout << "Enter quantity: ";
                cin >> quantity;
                
                bill = quantity * myPizzaShop->price[pizzaChoice];
                placeOrderTakeAwayCustomer(age, name, myPizzaShop->menu[pizzaChoice], quantity, bill);
                break;
            }
            
            case 2: {
                cout << "\n=== Home Delivery Order ===" << endl;
                cout << "Enter customer name: ";
                cin >> name;
                cout << "Enter customer age: ";
                cin >> age;
                
                cout << "\nDelivery Areas:" << endl;
                for (int i = 1; i <= 5; i++) {
                    cout << i << ". " << deliveryPoints[i] << endl;
                }
                cout << "Select delivery area (1-5): ";
                cin >> deliveryChoice;
                
                cout << "\nMenu:" << endl;
                for (int i = 1; i <= 10; i++) {
                    cout << i << ". " << myPizzaShop->menu[i] << " - " << myPizzaShop->price[i] << " RS" << endl;
                }
                
                cout << "Select pizza (1-10): ";
                cin >> pizzaChoice;
                cout << "Enter quantity: ";
                cin >> quantity;
                
                vector<int> distances = dijkstra(0);
                int deliveryCharges = distances[deliveryChoice] * 50;
                bill = quantity * myPizzaShop->price[pizzaChoice] + deliveryCharges;
                
                placeOrderHomeDeliveryCustomer(age, name, myPizzaShop->menu[pizzaChoice], 
                                            quantity, bill, deliveryPoints[deliveryChoice], 
                                            deliveryCharges, distances[deliveryChoice]);
                break;
            }
            
            case 3: {
                cout << "\n=== Dine-In Order ===" << endl;
                cout << "Enter customer name: ";
                cin >> name;
                cout << "Enter customer age: ";
                cin >> age;
                
                cout << "\nMenu:" << endl;
                for (int i = 1; i <= 10; i++) {
                    cout << i << ". " << myPizzaShop->menu[i] << " - " << myPizzaShop->price[i] << " RS" << endl;
                }
                
                cout << "Select pizza (1-10): ";
                cin >> pizzaChoice;
                cout << "Enter quantity: ";
                cin >> quantity;
                
                bill = quantity * myPizzaShop->price[pizzaChoice];
                placeOrderDineInCustomer(age, name, myPizzaShop->menu[pizzaChoice], quantity, bill);
                break;
            }
            
            case 4:
                serveOrderTakeAwayCustomer();
                break;
                
            case 5:
                serveOrderHomeDeliveryCustomer();
                break;
                
            case 6:
                serveOrderDineInCustomer();
                break;
                
            case 7:
                serveAllOrders();
                break;
                
            case 8:
                displayAllOrders();
                break;
                
            case 9:
                cout << "\n=== Served Orders ===" << endl;
                inOrder(root);
                break;
                
            case 10: {
                cout << "Enter customer name to search: ";
                cin >> name;
                servedCustomer* result = search(root, name);
                if (result) {
                    displayCustomer(result);
                } else {
                    cout << "Order not found!" << endl;
                }
                break;
            }
                
            case 11:
                deleteAllServedCustomers(root);
                root = NULL;
                cout << "Served orders cleared!" << endl;
                break;
                
            case 12:
                calculatePendingBills();
                break;
                
            case 13:
                cout << "Total Earnings: " << calculateTotalEarnings(root) << " RS" << endl;
                break;
                
            case 0:
                cout << "Exiting system..." << endl;
                break;
                
            default:
                cout << "Invalid choice!" << endl;
        }
    } while (choice != 0);
    
    // Clean up memory
    deleteAllServedCustomers(root);
    delete myPizzaShop;
    
    return 0;
}