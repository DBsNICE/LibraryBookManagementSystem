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

// File persistence
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
        fprintf(fp, "%d\n%s\n%.2f\n", m->id, m->name, m->fine);
    }
    fclose(fp);
}

void loadMembersFromFile() {
    FILE* fp = fopen("members.txt", "r");
    if (!fp) return;
    while (!feof(fp)) {
        Member* m = malloc(sizeof(Member));
        if (fscanf(fp, "%d\n%[^\n]\n%f\n", &m->id, m->name, &m->fine) == 3) {
            m->next = memberHead;
            memberHead = m;
        } else {
            free(m);
        }
    }
    fclose(fp);
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
    Book* newBook = malloc(sizeof(Book));
    printf("Enter Book ID: ");
    scanf("%d", &newBook->id);
    printf("Enter ISBN: ");
    scanf(" %[^\n]", newBook->isbn);
    printf("Enter Title: ");
    scanf(" %[^\n]", newBook->title);
    printf("Enter Author: ");
    scanf(" %[^\n]", newBook->author);
    printf("Enter Genre: ");
    scanf(" %[^\n]", newBook->genre);
    newBook->isBorrowed = 0;
    newBook->next = bookHead;
    bookHead = newBook;
    saveBooksToFile();
    printf("Book added successfully!\n");
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
    Book* temp = bookHead;
    while (temp) {
        printf("ID: %d | Title: %s | Author: %s | Genre: %s | Status: %s\n",
               temp->id, temp->title, temp->author, temp->genre,
               temp->isBorrowed ? "Borrowed" : "Available");
        temp = temp->next;
    }
}

void sortBooksByTitle() {
    if (!bookHead || !bookHead->next) return;
    Book *i, *j;
    for (i = bookHead; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            if (strcmp(i->title, j->title) > 0) {
                char tempTitle[100], tempAuthor[100], tempGenre[50];
                int tempId = i->id, tempBorrow = i->isBorrowed;
                strcpy(tempTitle, i->title); strcpy(tempAuthor, i->author); strcpy(tempGenre, i->genre);
                i->id = j->id; i->isBorrowed = j->isBorrowed;
                strcpy(i->title, j->title); strcpy(i->author, j->author); strcpy(i->genre, j->genre);
                j->id = tempId; j->isBorrowed = tempBorrow;
                strcpy(j->title, tempTitle); strcpy(j->author, tempAuthor); strcpy(j->genre, tempGenre);
            }
        }
    }
    printf("Books sorted by title.\n");
}

Book* binarySearchBook(const char* title) {
    Book* books[100];
    int count = 0;
    Book* temp = bookHead;
    while (temp) {
        books[count++] = temp;
        temp = temp->next;
    }
    int low = 0, high = count - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        int cmp = strcmp(books[mid]->title, title);
        if (cmp == 0) return books[mid];
        else if (cmp < 0) low = mid + 1;
        else high = mid - 1;
    }
    return NULL;
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

void searchBook() {
    char title[100];
    printf("Enter title to search: ");
    scanf(" %[^\n]", title);
    
    sortBooksByTitle();  // Ensure the list is sorted before binary search
    
    Book* found = binarySearchBook(title);
    if (found) {
        printf("Book Found:\n");
        printf("ID: %d | Title: %s | Author: %s | Genre: %s | Status: %s\n",
               found->id, found->title, found->author, found->genre,
               found->isBorrowed ? "Borrowed" : "Available");
    } else {
        printf("Book with title '%s' not found.\n", title);
    }
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

void adminMenu() {
    int choice;
    do {
        printf("\n--- Admin Panel ---\n");
        printf("1. Add Book\n2. Edit Book\n3. Delete Book\n4. View Books\n5. Sort Books\n6. Process Borrow Requests\n7. Edit Member\n8. Delete Member\n0. Back\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: addBook(); break;
            case 2: editBook(); break;
            case 3: deleteBook(); break;
            case 4: viewBooks(); break;
            case 5: sortBooksByTitle(); break;
            case 6: processBorrowRequests(); break;
            case 7: editMember(); break;
            case 8: deleteMember(); break;
        }
    } while (choice != 0);
}

void memberMenu() {
    int choice;
    do {
        printf("\n--- Member Panel ---\n");
        printf("1. Register\n2. Borrow Book\n3. Return Book\n4. Pay Fine\n5. Search Book\n0. Back\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: registerMember(); break;
            case 2: borrowBook(); break;
            case 3: returnBook(); break;
            case 4: payFine(); break;
            case 5: searchBook(); break;
        }
    } while (choice != 0);
}

int main() {
    srand(time(NULL));
    loadBooksFromFile();
    loadMembersFromFile();
    int choice;
    do {
        printf("\nLibrary System\n");
        printf("1. Admin\n2. Member\n0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: adminMenu(); break;
            case 2: memberMenu(); break;
        }
    } while (choice != 0);

    return 0;
}
