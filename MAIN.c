#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STUDENTS 100
#define MAX_ROOMS 10
#define NAME_LEN 50
#define TYPE_LEN 20

// --- Data Structures ---
typedef struct {
    char id[15];
    char name[NAME_LEN];
    int year_of_study;
    bool has_special_needs;
    char preferred_type[TYPE_LEN];
    int allocated_room_id; // -1 if not allocated (waiting list)
} Student;

typedef struct {
    int room_number;
    char type[TYPE_LEN];
    int capacity;
    int occupancy;
} Room;

// --- Global Memory ---
Student students[MAX_STUDENTS];
int student_count = 0;

Room rooms[MAX_ROOMS] = {
    {101, "Single", 1, 0},
    {102, "Single", 1, 0},
    {201, "Double", 2, 0},
    {202, "Double", 2, 0},
    {301, "Deluxe", 2, 0}
};
int room_count = 5;

// --- Function Prototypes ---
void registerStudent();
void processAllocations();
void searchAllocation();
void updateAllocation();
void deleteAllocation();
void displayAvailableRooms();
void generateReport();
void displayMenu();

// --- Main Execution Loop ---
int main() {
    int choice;
    do {
        displayMenu();
        printf("Enter your choice (1-7): ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }
        getchar(); // Clear newline buffer

        switch (choice) {
            case 1: registerStudent(); break;
            case 2: searchAllocation(); break;
            case 3: updateAllocation(); break;
            case 4: deleteAllocation(); break;
            case 5: displayAvailableRooms(); break;
            case 6: generateReport(); break;
            case 7: printf("Exiting system. Goodbye!\n"); break;
            default: printf("Invalid option. Please try again.\n");
        }
    } while (choice != 7);

    return 0;
}

void displayMenu() {
    printf("\n=============================================\n");
    printf("        HOSTEL ROOM ALLOCATION SYSTEM        \n");
    printf("=============================================\n");
    printf("1. Register Student & Run Allocation\n");
    printf("2. Search Room Allocation\n");
    printf("3. Update Allocation Details\n");
    printf("4. Delete Allocation Record\n");
    printf("5. Display Available Rooms\n");
    printf("6. Generate Hostel Allocation Report\n");
    printf("7. Exit\n");
    printf("=============================================\n");
}

// --- Rule-Based Allocation Engine ---
void processAllocations() {
    // Reset all room occupancies before recalculating priorities
    for (int i = 0; i < room_count; i++) {
        rooms[i].occupancy = 0;
    }
    for (int i = 0; i < student_count; i++) {
        students[i].allocated_room_id = -1;
    }

    // Process allocations layer by layer based on priority rules
    // Priority 1: Special Needs Students
    // Priority 2: Final Year Students (Year 4)
    // Priority 3: Regular Students
    for (int pass = 1; pass <= 3; pass++) {
        for (int i = 0; i < student_count; i++) {
            // Skip if student already has a room allocated
            if (students[i].allocated_room_id != -1) continue;

            bool matches_pass = false;
            if (pass == 1 && students[i].has_special_needs) matches_pass = true;
            else if (pass == 2 && !students[i].has_special_needs && students[i].year_of_study == 4) matches_pass = true;
            else if (pass == 3 && !students[i].has_special_needs && students[i].year_of_study < 4) matches_pass = true;

            if (!matches_pass) continue;

            // Attempt Step A: Match preferred room type with capacity
            bool allocated = false;
            for (int j = 0; j < room_count; j++) {
                if (strcasecmp(rooms[j].type, students[i].preferred_type) == 0 && rooms[j].occupancy < rooms[j].capacity) {
                    rooms[j].occupancy++;
                    students[i].allocated_room_id = rooms[j].room_number;
                    allocated = true;
                    break;
                }
            }

            // Attempt Step B: Fallback to any available room if preference is full
            if (!allocated) {
                for (int j = 0; j < room_count; j++) {
                    if (rooms[j].occupancy < rooms[j].capacity) {
                        rooms[j].occupancy++;
                        students[i].allocated_room_id = rooms[j].room_number;
                        allocated = true;
                        break;
                    }
                }
            }
        }
    }
}

// --- Core Features ---

void registerStudent() {
    if (student_count >= MAX_STUDENTS) {
        printf("Error: Maximum student registration capacity reached.\n");
        return;
    }

    Student s;
    char choice;

    printf("Enter Student ID: ");
    fgets(s.id, sizeof(s.id), stdin);
    s.id[strcspn(s.id, "\n")] = 0;

    printf("Enter Student Name: ");
    fgets(s.name, sizeof(s.name), stdin);
    s.name[strcspn(s.name, "\n")] = 0;

    printf("Enter Year of Study: ");
    scanf("%d", &s.year_of_study);
    getchar();

    printf("Does the student have special needs? (y/n): ");
    scanf(" %c", &choice);
    getchar();
    s.has_special_needs = (choice == 'y' || choice == 'Y');

    printf("Enter Preferred Room Type (Single/Double/Deluxe): ");
    fgets(s.preferred_type, sizeof(s.preferred_type), stdin);
    s.preferred_type[strcspn(s.preferred_type, "\n")] = 0;

    s.allocated_room_id = -1;
    students[student_count] = s;
    student_count++;

    // Re-evaluate allocations dynamically with the new student added
    processAllocations();

    // Check if this newly added student ended up on the waiting list
    if (students[student_count - 1].allocated_room_id == -1) {
        printf("\n⚠️ NOTICE: No suitable room available. Student has been placed on the WAITING LIST.\n");
    } else {
        printf("\nSuccess: Student registered and allocated to Room %d.\n", students[student_count - 1].allocated_room_id);
    }
}

void searchAllocation() {
    char search_id[15];
    printf("Enter Student ID to search: ");
    fgets(search_id, sizeof(search_id), stdin);
    search_id[strcspn(search_id, "\n")] = 0;

    for (int i = 0; i < student_count; i++) {
        if (strcmp(students[i].id, search_id) == 0) {
            printf("\n--- Allocation Details ---\n");
            printf("ID: %s\nName: %s\nYear: %d\nSpecial Needs: %s\n",
                   students[i].id, students[i].name, students[i].year_of_study,
                   students[i].has_special_needs ? "Yes" : "No");
            if (students[i].allocated_room_id == -1) {
                printf("Status: WAITING LIST\n");
            } else {
                printf("Allocated Room: %d\n", students[i].allocated_room_id);
            }
            return;
        }
    }
    printf("Error: Student record not found.\n");
}

void updateAllocation() {
    char search_id[15];
    printf("Enter Student ID to update details: ");
    fgets(search_id, sizeof(search_id), stdin);
    search_id[strcspn(search_id, "\n")] = 0;

    for (int i = 0; i < student_count; i++) {
        if (strcmp(students[i].id, search_id) == 0) {
            printf("Current Preference: %s. Enter New Room Preference: ", students[i].preferred_type);
            fgets(students[i].preferred_type, sizeof(students[i].preferred_type), stdin);
            students[i].preferred_type[strcspn(students[i].preferred_type, "\n")] = 0;

            processAllocations();
            printf("Record updated and allocations recalculated successfully.\n");
            return;
        }
    }
    printf("Error: Student record not found.\n");
}

void deleteAllocation() {
    char search_id[15];
    printf("Enter Student ID to remove allocation/record: ");
    fgets(search_id, sizeof(search_id), stdin);
    search_id[strcspn(search_id, "\n")] = 0;

    for (int i = 0; i < student_count; i++) {
        if (strcmp(students[i].id, search_id) == 0) {
            // Shift remaining records down
            for (int j = i; j < student_count - 1; j++) {
                students[j] = students[j + 1];
            }
            student_count--;
            processAllocations();
            printf("Record removed successfully. Allocation updates applied.\n");
            return;
        }
    }
    printf("Error: Student record not found.\n");
}

void displayAvailableRooms() {
    printf("\n--- Available Rooms ---\n");
    printf("%-10s %-15s %-10s\n", "Room No", "Type", "Free Beds");
    bool found = false;
    for (int i = 0; i < room_count; i++) {
        if (rooms[i].occupancy < rooms[i].capacity) {
            printf("%-10d %-15s %-10d\n", rooms[i].room_number, rooms[i].type, (rooms[i].capacity - rooms[i].occupancy));
            found = true;
        }
    }
    if (!found) {
        printf("No rooms available at the moment.\n");
    }
}

void generateReport() {
    printf("\n========================================================\n");
    printf("               HOSTEL ALLOCATION REPORT                 \n");
    printf("========================================================\n");

    printf("\n--- Fully Occupied Rooms ---\n");
    bool has_full = false;
    for (int i = 0; i < room_count; i++) {
        if (rooms[i].occupancy == rooms[i].capacity) {
            printf("Room %d (%s) - Occupancy: %d/%d [FULLY OCCUPIED]\n",
                   rooms[i].room_number, rooms[i].type, rooms[i].occupancy, rooms[i].capacity);
            has_full = true;
        }
    }
    if (!has_full) printf("None\n");

    printf("\n--- Partially Occupied/Empty Rooms ---\n");
    for (int i = 0; i < room_count; i++) {
        if (rooms[i].occupancy < rooms[i].capacity) {
            printf("Room %d (%s) - Occupancy: %d/%d\n",
                   rooms[i].room_number, rooms[i].type, rooms[i].occupancy, rooms[i].capacity);
        }
    }

    printf("\n--- Waiting List Summary ---\n");
    bool has_waiting = false;
    for (int i = 0; i < student_count; i++) {
if (students[i].allocated_room_id == -1) {
        printf("Student ID: %-10s | Name: %-20s | Priority Level: %s\n",students[i].id, students[i].name,students[i].has_special_needs ? "High (Special Needs)" :(students[i].year_of_study == 4 ? "Medium (Final Year)" : "Standard"));has_waiting = true;}}
if (!has_waiting) printf("No students on the waiting list.\n");printf("========================================================\n");}
