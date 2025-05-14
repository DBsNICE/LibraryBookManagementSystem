#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Book Linked List Node
typedef struct Book {
    int id;
    char title[100];
    char author[100];
    char genre[50];
    int isBorrowed;
    int borrowCount;
    char isbn[20];
    struct Book* next;
} Book;

// Member Linked List Node
typedef struct Member {
    int id;
    char name[100];
    float fine;
    int borrowHistoryCount;
    struct Member* next;
} Member;

// Borrow Request Queue Node
typedef struct BorrowRequest {
    int memberId;
    int bookId;
    char isbn[20];
    time_t borrowTime;
    struct BorrowRequest* next;
} BorrowRequest;

Book* bookHead = NULL;
Member* memberHead = NULL;
BorrowRequest *front = NULL;
BorrowRequest *rear = NULL;
Member* copyMemberList();
void sortMembersByBorrowCount(Member *head);
void swapMemberData(Member* a, Member* b);

Book* copyBookList();
void sortBooksByBorrowCount(Book *head);
void swapBookData(Book* a, Book* b);
void logAction(const char* action);
void freeBookList(Book* head);
void freeMemberList(Member* head);

char line[10];
const char* adminusername = "admin";
const char* adminpassword = "admin123";
int nextBookID = 1000;

Book* copyBookList() {
    Book* head = NULL, *tail = NULL;
    for (Book* b = bookHead; b != NULL; b = b->next) {
        Book* copy = malloc(sizeof(Book));
        *copy = *b;
        copy->next = NULL;
        if (!head) head = tail = copy;
        else {
            tail->next = copy;
            tail = copy;
        }
    }
    return head;
}

void swapBookData(Book* a, Book* b) {
    int id = a->id;
    char title[100], author[100], genre[50], isbn[20];
    int isBorrowed = a->isBorrowed;
    int borrowCount = a->borrowCount;

    strcpy(title, a->title);
    strcpy(author, a->author);
    strcpy(genre, a->genre);
    strcpy(isbn, a->isbn);

    a->id = b->id;
    strcpy(a->title, b->title);
    strcpy(a->author, b->author);
    strcpy(a->genre, b->genre);
    strcpy(a->isbn, b->isbn);
    a->isBorrowed = b->isBorrowed;
    a->borrowCount = b->borrowCount;

    b->id = id;
    strcpy(b->title, title);
    strcpy(b->author, author);
    strcpy(b->genre, genre);
    strcpy(b->isbn, isbn);
    b->isBorrowed = isBorrowed;
    b->borrowCount = borrowCount;
}

void saveBooksToFile() {
    FILE* fp = fopen("books.txt", "w");
    for (Book* b = bookHead; b != NULL; b = b->next) {
        fprintf(fp, "%d\n%s\n%s\n%s\n%d\n", b->id, b->title, b->author, b->genre, b->isBorrowed);
    }
    fclose(fp);
}

void loadBooksFromFile() {
    FILE* fp = fopen("books.txt", "r");
    if (!fp) return;
    while (!feof(fp)) {
        Book* b = malloc(sizeof(Book));
        if (fscanf(fp, "%d\n%[^\n]\n%[^\n]\n%[^\n]\n%d\n", &b->id, b->title, b->author, b->genre, &b->isBorrowed) == 5) {
            b->next = bookHead;
            bookHead = b;
        } else {
            free(b);
        }
    }
    fclose(fp);
}

void saveMembersToFile() {
    FILE* fp = fopen("members.txt", "w");
    for (Member* m = memberHead; m != NULL; m = m->next) {
        fprintf(fp, "%d\n%s\n%.2f\n%d\n", m->id, m->name, m->fine, m->borrowHistoryCount);
    }
    fclose(fp);
}

void loadMembersFromFile() {
    FILE* fp = fopen("members.txt", "r");
    if (!fp) return;

    while (1) {
        Member* m = malloc(sizeof(Member));
        if (!m) break;
        if (fscanf(fp, "%d\n%[^\n]\n%f\n%d\n", &m->id, m->name, &m->fine, &m->borrowHistoryCount) != 4){
        if (fscanf(fp, "%d\n%[^\n]\n%f\n", &m->id, m->name, &m->fine) != 3) {
            free(m);
            break;  
        }

        m->borrowHistoryCount = 0;
        m->next = memberHead;
        memberHead = m;
    }
    }
    fclose(fp);
}

void saveNextBookID() {
    FILE* fp = fopen("bookid.txt", "w");
    if (fp) {
        fprintf(fp, "%d", nextBookID);
        fclose(fp);
    }
}

void loadNextBookID() {
    FILE* fp = fopen("bookid.txt", "r");
    if (fp) {
        fscanf(fp, "%d", &nextBookID);
        fclose(fp);
    }
}

void generateMostActiveMembersReport() {
    Member *sortedMembers = copyMemberList();
    sortMembersByBorrowCount(sortedMembers);

    printf("Most Active Members:\n");
    printf("%-20s %-10s\n", "Name", "Borrows");
    Member *temp = sortedMembers;
    while (temp) {
        printf("%-20s %-10d\n", temp->name, temp->borrowHistoryCount);
        temp = temp->next;
    }
}

void sortMembersByBorrowCount(Member *head) {
    for (Member *i = head; i != NULL; i = i->next) {
        for (Member *j = i->next; j != NULL; j = j->next) {
            if (j->borrowHistoryCount > i->borrowHistoryCount) {
                swapMemberData(i, j);
            }
        }
    }
}

Book* findBookById(int id) {
    Book* temp = bookHead;
    while (temp) {
        if (temp->id == id) return temp;
        temp = temp->next;
    }
    return NULL;
}

Member* findMemberById(int id) {
    Member* temp = memberHead;
    while (temp) {
        if (temp->id == id) return temp;
        temp = temp->next;
    }
    return NULL;
}

void enqueue(int memberId, int bookId, char isbn[]) {
    BorrowRequest *newRequest = malloc(sizeof(BorrowRequest));
    newRequest->memberId = memberId;
    newRequest->bookId = bookId;
    strcpy(newRequest->isbn, isbn);
    newRequest->next = NULL;
    if (rear == NULL) {
        front = rear = newRequest;
    } else {
        rear->next = newRequest;
        rear = newRequest;
    }
}

BorrowRequest *dequeue() {
    if (front == NULL) return NULL;

    BorrowRequest *temp = front;
    front = front->next;
    if (front == NULL) rear = NULL;
    return temp;
}

int isQueueEmpty() {
    return front == NULL;
}

void processBorrowRequests() {
   
    while (front != NULL) {
        BorrowRequest* req = front;
        Book* book = findBookById(req->bookId);
        Member* member = findMemberById(req->memberId);
        if (book && member && book->isBorrowed == 0) {
            book->isBorrowed = 1;
            printf("Book ID %d borrowed by Member ID %d.\n", book->id, member->id);
            book->borrowCount++;
            member->borrowHistoryCount++;
        } else {
            printf("Request for Book ID %d by Member ID %d could not be processed.\n", req->bookId, req->memberId);
        }
        front = front->next;
        free(req);
    }
    rear = NULL;
    saveBooksToFile();
}

void returnBook() {
    int bookId, memberId;
    printf("Enter Book ID to return: ");
    scanf("%d", &bookId);
    printf("Enter Member ID: ");
    scanf("%d", &memberId);
    Book* book = findBookById(bookId);
    Member* member = findMemberById(memberId);

    if (book && member && book->isBorrowed) {
        book->isBorrowed = 0;
        int overdueDays = rand() % 10;
        if (overdueDays > 5) {
            float fine = (overdueDays - 5) * 1.0f;
            member->fine += fine;
            printf("Book returned late. Fine added: $%.2f\n", fine);
        } else {
            printf("Book returned on time.\n");
        }
        saveBooksToFile();
        saveMembersToFile();
    } else {
        printf("Return failed. Book may not be borrowed or IDs invalid.\n");
    }
}

void addBook() {
    Book* newBook = (Book*)malloc(sizeof(Book));
    newBook->id = nextBookID++;  // Auto-increment ID
    printf("Enter book title: ");
    scanf(" %[^\n]", newBook->title);
    printf("Enter author: ");
    scanf(" %[^\n]", newBook->author);
    newBook->isBorrowed = 1;
    newBook->borrowCount = 0;

    // Insert into list
    newBook->next = bookHead;
    bookHead = newBook;

    printf("Book added with ID: %d\n", newBook->id);
}

void editBook() {
    int id;
    printf("Enter Book ID to edit: ");
    scanf("%d", &id);
    Book* book = findBookById(id);
    if (book) {
        printf("Enter new title: ");
        scanf(" %[^\n]", book->title);
        printf("Enter new author: ");
        scanf(" %[^\n]", book->author);
        printf("Enter new genre: ");
        scanf(" %[^\n]", book->genre);
        saveBooksToFile();
        printf("Book updated.\n");
    } else {
        printf("Book not found.\n");
    }
}

void deleteBook() {
    int id;
    printf("Enter Book ID to delete: ");
    scanf("%d", &id);
    Book **ptr = &bookHead;
    while (*ptr) {
        if ((*ptr)->id == id) {
            Book* tmp = *ptr;
            *ptr = (*ptr)->next;
            free(tmp);
            saveBooksToFile();
            printf("Book deleted.\n");
            return;
        }
        ptr = &(*ptr)->next;
    }
    printf("Book not found.\n");
}

void viewBooks() {
        // Count books
    int count = 0;
    Book* temp = bookHead;
    while (temp) {
        count++;
        temp = temp->next;
    }

    // Copy to array
    Book** bookArray = malloc(sizeof(Book*) * count);
    temp = bookHead;
    for (int i = 0; i < count; i++) {
        bookArray[i] = temp;
        temp = temp->next;
    }

    // Sort by ID
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (bookArray[i]->id > bookArray[j]->id) {
                Book* tmp = bookArray[i];
                bookArray[i] = bookArray[j];
                bookArray[j] = tmp;
            }
        }
    }

    // Display
    for (int i = 0; i < count; i++) {
    printf("ID: %d, Title: %s, Author: %s, Genre: %s, Available: %s\n",
        bookArray[i]->id,
        bookArray[i]->title,
        bookArray[i]->author,
        bookArray[i]->genre,
        bookArray[i]->isBorrowed ? "Yes" : "No");
}

free(bookArray);
}

Book* binarySearchBook(const char* title) {
    Book* books[100];
    int count = 0;
    Book* temp = bookHead;

  
    while (temp && count < 100) {
        books[count++] = temp;
        temp = temp->next;
    }

    // Sort the array by title 
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcasecmp(books[i]->title, books[j]->title) > 0) {
                Book* tmp = books[i];
                books[i] = books[j];
                books[j] = tmp;
            }
        }
    }

    // Binary search
    int low = 0, high = count - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        int cmp = strcasecmp(books[mid]->title, title); 
        if (cmp == 0) return books[mid];
        else if (cmp < 0) low = mid + 1;
        else high = mid - 1;
    }
    return NULL;
}

void searchBook() {
    int choice, id;
    char input[100];
    printf("\nSearch Book By:\n1. ID\n2. Title\n3. Author\nChoice: ");
    scanf("%d", &choice);
    getchar();  // newline flush 
    switch (choice) {
        case 1:
            printf("Enter Book ID: ");
            scanf("%d", &id);
            Book* book = findBookById(id);
            if (book) {
                printf("Book Found: ID: %d, Title: %s, Author: %s, Genre: %s\n", 
                       book->id, book->title, book->author, book->genre);
            } else {
                printf("Book with ID %d not found.\n", id);
            }
            break;
        case 2:
            printf("Enter Book Title: ");
            scanf(" %[^\n]", input);  // Read full title with spaces
            book = binarySearchBook(input);  // Binary search for title
            if (book) {
                printf("Book Found: ID: %d, Title: %s, Author: %s, Genre: %s\n", 
                       book->id, book->title, book->author, book->genre);
            } else {
                printf("Book with title \"%s\" not found.\n", input);
            }
            break;
        case 3:
            printf("Enter Author Name: ");
            scanf(" %[^\n]", input);  // Read full author name with spaces
            int found = 0;
            for (book = bookHead; book != NULL; book = book->next) {
                if (strstr(book->author, input)) {  // Search by author name
                    printf("Book Found: ID: %d, Title: %s, Author: %s, Genre: %s\n", 
                           book->id, book->title, book->author, book->genre);
                    found = 1;
                }
            }
            if (!found) {
                printf("No books found by author \"%s\".\n", input);
            }
            break;
        default:
            printf("Invalid choice.\n");
            break;
    }
}

void registerMember() {
    Member* newMem = malloc(sizeof(Member));
    printf("Enter Member ID: ");
    scanf("%d", &newMem->id);
    printf("Enter Name: ");
    scanf(" %[^\n]", newMem->name);
    newMem->fine = 0;
    newMem->next = memberHead;
    memberHead = newMem;
    saveMembersToFile();
    printf("Member registered!\n");
}

void editMember() {
    int id;
    printf("Enter Member ID to edit: ");
    scanf("%d", &id);
    Member* mem = findMemberById(id);
    if (mem) {
        printf("Enter new name: ");
        scanf(" %[^\n]", mem->name);
        saveMembersToFile();
        printf("Member updated.\n");
    } else {
        printf("Member not found.\n");
    }
}

void deleteMember() {
    int id;
    printf("Enter Member ID to delete: ");
    scanf("%d", &id);
    Member **ptr = &memberHead;
    while (*ptr) {
        if ((*ptr)->id == id) {
            Member* tmp = *ptr;
            *ptr = (*ptr)->next;
            free(tmp);
            saveMembersToFile();
            printf("Member deleted.\n");
            return;
        }
        ptr = &(*ptr)->next;
    }
    printf("Member not found.\n");
}

void memberlist() {
    if (!memberHead) {
        printf("No members registered.\n");
        return;  
    }

    printf("\n--- Member List ---\n");
    Member* temp = memberHead;
    while (temp) {
        printf("ID: %d | Name: %s | Fine: $%.2f | Borrows: %d\n", 
               temp->id, temp->name, temp->fine, temp->borrowHistoryCount);
        temp = temp->next;
    }
}

void borrowBook() {
    int memberId, bookId;
    printf("Enter Member ID: ");
    scanf("%d", &memberId);
    printf("Enter Book ID: ");
    scanf("%d", &bookId);

    Book* book = findBookById(bookId);
    if (!book) {
        printf("Book not found.\n");
        return;
    }
    enqueue(memberId, bookId, book->isbn);// Pass ISBN properly
    printf("Borrow request enqueued.\n");
}

void payFine() {
    int memberId;
    printf("Enter Member ID: ");
    scanf("%d", &memberId);
    Member* member = findMemberById(memberId);
    if (member && member->fine > 0) {
        printf("Outstanding Fine: %.2f\n", member->fine);
        member->fine = 0;
        saveMembersToFile();
        printf("Fine cleared.\n");
    } else {
        printf("No fine due.\n");
    }
}

void swapMemberData(Member* a, Member* b) {
    int id = a->id;
    char name[100];
    float fine = a->fine;
    int history = a->borrowHistoryCount;

    strcpy(name, a->name);

    a->id = b->id;
    strcpy(a->name, b->name);
    a->fine = b->fine;
    a->borrowHistoryCount = b->borrowHistoryCount;

    b->id = id;
    strcpy(b->name, name);
    b->fine = fine;
    b->borrowHistoryCount = history;
}

Member* copyMemberList() {
    Member* head = NULL, *tail = NULL;
    for (Member* m = memberHead; m != NULL; m = m->next) {
        Member* copy = malloc(sizeof(Member));
        *copy = *m;
        copy->next = NULL;
        if (!head) head = tail = copy;
        else {
            tail->next = copy;
            tail = copy;
        }
    }
    return head;
}

void generateTopBorrowedBooksReport() {
    Book *sortedBooks = copyBookList();
    sortBooksByBorrowCount(sortedBooks);

    printf("Top Borrowed Books:\n");
    printf("%-20s %-20s %-10s %s\n", "Title", "Author", "Count", "ISBN");
    Book *temp = sortedBooks;
    while (temp) {
        printf("%-20s %-20s %-10d %s\n", temp->title, temp->author, temp->borrowCount, temp->isbn);
        temp = temp->next;
    }
}

void sortBooksByBorrowCount(Book *head) {
    for (Book *i = head; i != NULL; i = i->next) {
        for (Book *j = i->next; j != NULL; j = j->next) {
            if (j->borrowCount > i->borrowCount) {
                swapBookData(i, j);
            }
        }
    }
}

void logAction(const char* action) {
    FILE* logFile = fopen("log.csv", "a");
    if (logFile) {
        time_t now = time(NULL);
        char* timeStr = ctime(&now);
        timeStr[strcspn(timeStr, "\n")] = '\0'; 
        fprintf(logFile, "%s,%s\n", timeStr, action);
        fclose(logFile);
    }
}

int adminlogin() {
    char username[50];
    char password[50];
    printf("=== Admin Login ===\n");
    printf("Username: ");
    scanf(" %s", username);
    printf("Password: ");
    scanf(" %s", password);

    if (strcmp(username, adminusername) == 0 && strcmp(password, adminpassword) == 0) {
        printf("Login successful Welcome Admin!\n");
        return 1;
    } else {
        printf("Login failed. Invalid username or password.\n");
        return 0;
    }
}

Member* memberIDlogin() {
    int id;
    printf("Enter your Member ID: ");
    scanf("%d", &id);

    Member* current = memberHead;
    while (current != NULL) {
        if (current->id == id) {
            printf("Welcome, %s!\n", current->name);
            return current;
        }
        current = current->next;
    }

    printf("Member ID not found.\n");
    return NULL;
}

void adminMenu() {
    int choice;
    do {
        printf("\n--- Admin Panel ---\n");
        printf("1. Add Book\n2. Edit Book\n3. Delete Book\n4. View Books\n5. Search Books\n6. Process Borrow Requests\n7. RegisterMember\n8. MemberList\n9. Edit Member\n10. Delete Member\n0. Back\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: addBook(); break;
            case 2: {
            viewBooks();
            editBook(); 
            break;
            }
            case 3: {
            viewBooks();
            deleteBook(); 
            break;
            }
            case 4: viewBooks(); break;
            case 5: searchBook(); break; 
            case 6: processBorrowRequests(); break;
            case 7: {
            registerMember();
            break;
            }
            case 8: {
            memberlist();
            editMember(); 
            break;
            }
            case 9: {
            memberlist();
            deleteMember(); 
            break;
            }
        }
    } while (choice != 0);
}

void memberMenu() {
    Member* member = memberIDlogin();
if (member == NULL) {
    return; 
}
    int choice;
    do {
        printf("\n--- Member Panel ---\n");
        printf("1. Register\n2. Borrow Book\n3. Return Book\n4. Pay Fine\n5. Search Book\n0. Back\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: registerMember(); break;
            case 2: {
                viewBooks();
                borrowBook(); 
                break;
            }
            case 3: returnBook(); break;
            case 4: payFine(); break;
            case 5: searchBook(); break;
        }
    } while (choice != 0);
}

int main() {
    loadBooksFromFile();
    loadMembersFromFile();
    loadNextBookID();
    char input[10];
    int choice = -1;

    while (1) {
        printf("\nLibrary System\n");
        printf("1. Admin\n2. Member\n0. Exit\nEnter choice: ");

        scanf(" %10s", input);
        choice = atoi(input); //converts a string to number int


        switch (choice) {
            case 1: {
                adminlogin();
                adminMenu();
                break;
            }
            case 2: 
                memberMenu();
                break;
            case 0:
                return 1;
            default:
                printf("Invalid Input.\n");
                break;
        }
    }

    return 0;
}
