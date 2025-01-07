#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "students.txt"
#define MAX_NAME 50
#define MAX_CLASSE 10
#define MAX_NOTES 4

// Button identifiers
#define BTN_ADD 101
#define BTN_SEARCH 102
#define BTN_MODIFY 103
#define BTN_DELETE 104
#define BTN_EXTRACT 105
#define BTN_REORGANIZE 106
#define BTN_EXIT 107
#define BTN_RESET 108
#define BTN_SUBMIT 109
#define BTN_SUBMIT_ADD 201
#define BTN_SUBMIT_SEARCH 202
#define BTN_SUBMIT_MODIFY 203
#define BTN_SUBMIT_DELETE 204
#define BTN_SUBMIT_Reorganize 205
#define BTN_EXTRACT_SUBMIT 206
#define BTN_SUBMIT_SEARCH2 207

long global_record_position = -1;

// Coefficients
const int coefficients[MAX_NOTES] = {4, 3, 2, 5};

// Student structure
typedef struct {
    int numero_inscription;
    char full_name[MAX_NAME];
    int annee_naissance;
    char classe[MAX_CLASSE];
    float notes[MAX_NOTES];
    float moyenne;
    int supprime;// if 1 il est supprime logiquement if 0 il est pas supprim
} Etudiant;


// Global handles for input fields
HWND hInputFields[10];
HWND hMainMenu;

// Function prototypes
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ShowMainMenu(HWND hwnd);
void ShowAddStudentInterface(HWND hwnd);
void ShowSearchStudentInterface(HWND hwnd);
void ShowModifyStudentInterface(HWND hwnd);
void SHOWSEARCHSTUDENTRESULTINTERFACE(HWND hwnd);
void ShowDeleteStudentInterface(HWND hwnd);
void ShowExtractByClassInterface(HWND hwnd);
void ShowReorganizeInterface(HWND hwnd);
void ResetFields(HWND parent);
void ShowSplashScreen(HINSTANCE hInstance);
void ClearChildWindows(HWND parent);
void HandleSearchStudent(HWND hwnd);
void HandleAddStudent(HWND hwnd);
void HandleReorganize(HWND hwnd);
void HandleDeleteStudent(HWND hwnd);
void HandleModifyStudent(HWND hwnd);
void HandleSubmitModifyStudent(HWND hwnd);
void HandleExtractByClass(HWND hwnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ShowSplashScreen(hInstance);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "StudentManagement";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Failed to register window class!", "Error", MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindow(
        "StudentManagement", "Student Management System",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        MessageBox(NULL, "Failed to create window!", "Error", MB_ICONERROR);
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

void ShowSplashScreen(HINSTANCE hInstance) {
    // Create a simple splash screen window
    HWND hSplash = CreateWindow(
        "STATIC", NULL,
        WS_POPUP | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, // Splash screen dimensions
        NULL, NULL, hInstance, NULL
    );

    if (!hSplash) return;

    // Center the splash screen
    RECT desktop;
    GetWindowRect(GetDesktopWindow(), &desktop);
    int x = (desktop.right - 600) / 2;
    int y = (desktop.bottom - 400) / 2;
    SetWindowPos(hSplash, NULL, x, y, 600, 400, SWP_NOZORDER | SWP_SHOWWINDOW);

    // Display an animated logo (example: using a static image)
    HBITMAP hLogo = (HBITMAP)LoadImage(
        NULL, "1LOGO3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE
    );
    if (hLogo) {
        HWND hLogoControl = CreateWindow(
            "STATIC", NULL,
            WS_CHILD | WS_VISIBLE | SS_BITMAP,
            0, 0, 600, 400, hSplash, NULL, hInstance, NULL
        );
        SendMessage(hLogoControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hLogo);
    }

    // Pause for 4 seconds
    Sleep(4000);

    // Destroy the splash screen
    DestroyWindow(hSplash);
}

void ClearChildWindows(HWND parent) {
    HWND child = GetWindow(parent, GW_CHILD);
    while (child != NULL) {
        HWND nextChild = GetNextWindow(child, GW_HWNDNEXT);
        DestroyWindow(child);
        child = nextChild;
    }
}

void ShowMainMenu(HWND hwnd) {
    ClearChildWindows(hwnd);
    CreateWindow("BUTTON", "Add Student", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 50, 50, 200, 40, hwnd, (HMENU)BTN_ADD, NULL, NULL);
    CreateWindow("BUTTON", "Search Student", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 50, 100, 200, 40, hwnd, (HMENU)BTN_SEARCH, NULL, NULL);
    CreateWindow("BUTTON", "Modify Student", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 50, 150, 200, 40, hwnd, (HMENU)BTN_MODIFY, NULL, NULL);
    CreateWindow("BUTTON", "Delete Student", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 50, 200, 200, 40, hwnd, (HMENU)BTN_DELETE, NULL, NULL);
    CreateWindow("BUTTON", "Extract By Class", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 50, 250, 200, 40, hwnd, (HMENU)BTN_EXTRACT, NULL, NULL);
    CreateWindow("BUTTON", "Reorganize Data", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 50, 300, 200, 40, hwnd, (HMENU)BTN_REORGANIZE, NULL, NULL);
    CreateWindow("BUTTON", "Exit", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 50, 350, 200, 40, hwnd, (HMENU)BTN_EXIT, NULL, NULL);
}

void ShowAddStudentInterface(HWND hwnd) {
    int offsetX = 400; // Position elements to the right of the menu

    // Add student input fields
    CreateWindow("STATIC", "Full Name:", WS_VISIBLE | WS_CHILD,
                 offsetX, 20, 150, 20, hwnd, NULL, NULL, NULL);
    hInputFields[0] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                   offsetX + 160, 20, 200, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("STATIC", "Birth Year:", WS_VISIBLE | WS_CHILD,
                 offsetX, 60, 150, 20, hwnd, NULL, NULL, NULL);
    hInputFields[1] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                   offsetX + 160, 60, 200, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("STATIC", "Matricule:", WS_VISIBLE | WS_CHILD,
                 offsetX, 100, 150, 20, hwnd, NULL, NULL, NULL);
    hInputFields[2] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                   offsetX + 160, 100, 200, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("STATIC", "Class:", WS_VISIBLE | WS_CHILD,
                 offsetX, 140, 150, 20, hwnd, NULL, NULL, NULL);
    hInputFields[3] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                   offsetX + 160, 140, 200, 20, hwnd, NULL, NULL, NULL);

    // Grades
    for (int i = 0; i < MAX_NOTES; i++) {
        char label[30];
        sprintf(label, "Grade %d:", i + 1);
        CreateWindow("STATIC", label, WS_VISIBLE | WS_CHILD,
                     offsetX, 180 + (i * 40), 150, 20, hwnd, NULL, NULL, NULL);
        hInputFields[4 + i] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                           offsetX + 160, 180 + (i * 40), 200, 20, hwnd, NULL, NULL, NULL);
    }

    // Buttons
    CreateWindow("BUTTON", "Submit", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 offsetX + 20, 400, 150, 30, hwnd, (HMENU)BTN_SUBMIT_ADD, NULL, NULL);
    CreateWindow("BUTTON", "Reset", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 offsetX + 200, 400, 150, 30, hwnd, (HMENU)BTN_RESET, NULL, NULL);
}

void ShowSearchStudentInterface(HWND hwnd) {
    int offsetX = 400; // Position to the right of the menu

    // Add search input fields and buttons
    CreateWindow("STATIC", "Enter Matricule:", WS_VISIBLE | WS_CHILD,
                 offsetX, 50, 150, 20, hwnd, NULL, NULL, NULL);
    hInputFields[0] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                   offsetX + 160, 50, 200, 20, hwnd, NULL, NULL, NULL);
    CreateWindow("BUTTON", "Search", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 offsetX, 90, 150, 30, hwnd, (HMENU)BTN_SUBMIT_SEARCH2, NULL, NULL);
    CreateWindow("BUTTON", "Back", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 offsetX + 200, 90, 150, 30, hwnd, (HMENU)BTN_RESET, NULL, NULL);
}

void ResetFields(HWND parent) {
    for (int i = 0; i < 10; i++) {
        if (hInputFields[i]) {
            SetWindowText(hInputFields[i], "");
        }
    }
}

Etudiant searchedStudent;
void SHOWSEARCHSTUDENTRESULTINTERFACE(HWND hwnd) {
    if (searchedStudent.numero_inscription == 0) {
        MessageBox(hwnd, "No valid search result to display!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Create a result string with the student's details
    char result[1024];
    sprintf(result, "Search Result:\n"
                    "Matricule: %d\n"
                    "Name: %s\n"
                    "Birth Year: %d\n"
                    "Class: %s\n"
                    "Grades: %.2f, %.2f, %.2f, %.2f\n"
                    "Average: %.2f\n",
            searchedStudent.numero_inscription, searchedStudent.full_name,
            searchedStudent.annee_naissance, searchedStudent.classe,
            searchedStudent.notes[0], searchedStudent.notes[1], searchedStudent.notes[2], searchedStudent.notes[3],
            searchedStudent.moyenne);

    // Display the result in a message box
    MessageBox(hwnd, result, "Search Result", MB_OK | MB_ICONINFORMATION);
}

void ShowModifyStudentInterface(HWND hwnd) {
    ClearChildWindows(hwnd);

    // Modify student interface
    CreateWindow("STATIC", "Enter Matricule to Modify:", WS_VISIBLE | WS_CHILD,
                 20, 20, 200, 20, hwnd, NULL, NULL, NULL);
    hInputFields[0] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                   230, 20, 200, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("BUTTON", "Fetch Details", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 450, 20, 150, 30, hwnd, (HMENU)BTN_SUBMIT_SEARCH, NULL, NULL);

    // Display fetched student details
    CreateWindow("STATIC", "Full Name:", WS_VISIBLE | WS_CHILD,
                 20, 80, 150, 20, hwnd, NULL, NULL, NULL);
    hInputFields[1] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                   230, 80, 200, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("STATIC", "Birth Year:", WS_VISIBLE | WS_CHILD,
                 20, 120, 150, 20, hwnd, NULL, NULL, NULL);
    hInputFields[2] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                   230, 120, 200, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("STATIC", "Matricule:", WS_VISIBLE | WS_CHILD,
                 20, 160, 150, 20, hwnd, NULL, NULL, NULL);
    hInputFields[3] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                   230, 160, 200, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("STATIC", "Class:", WS_VISIBLE | WS_CHILD,
                 20, 200, 150, 20, hwnd, NULL, NULL, NULL);
    hInputFields[4] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                   230, 200, 200, 20, hwnd, NULL, NULL, NULL);

    // Grades
    for (int i = 0; i < MAX_NOTES; i++) {
        char label[30];
        sprintf(label, "Grade %d:", i + 1);
        CreateWindow("STATIC", label, WS_VISIBLE | WS_CHILD,
                     20, 240 + (i * 40), 150, 20, hwnd, NULL, NULL, NULL);
        hInputFields[5 + i] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                           230, 240 + (i * 40), 200, 20, hwnd, NULL, NULL, NULL);
    }

    // Buttons
    CreateWindow("BUTTON", "Submit", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 50, 440, 150, 30, hwnd, (HMENU)BTN_SUBMIT_MODIFY, NULL, NULL);
    CreateWindow("BUTTON", "Reset", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 230, 440, 150, 30, hwnd, (HMENU)BTN_RESET, NULL, NULL);
}

void ShowDeleteStudentInterface(HWND hwnd) {
    ClearChildWindows(hwnd);

    // Delete student interface
    CreateWindow("STATIC", "Enter Matricule to Delete:", WS_VISIBLE | WS_CHILD,
                 20, 20, 200, 20, hwnd, NULL, NULL, NULL);
    hInputFields[0] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                   230, 20 , 200, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("BUTTON", "Delete", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 20, 60, 150, 30, hwnd, (HMENU)BTN_SUBMIT_DELETE, NULL, NULL);
    CreateWindow("BUTTON", "Back", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 200, 60, 150, 30, hwnd, (HMENU)BTN_RESET, NULL, NULL);
}

void ShowExtractByClassInterface(HWND hwnd) {
    ClearChildWindows(hwnd);

    // Extract by class interface
    CreateWindow("STATIC", "Enter Class:", WS_VISIBLE | WS_CHILD,
                 20, 20, 150, 20, hwnd, NULL, NULL, NULL);
    hInputFields[0] = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                   200, 20, 200, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("BUTTON", "Extract", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 20, 60, 150, 30, hwnd, (HMENU)BTN_EXTRACT_SUBMIT, NULL, NULL);
    CreateWindow("BUTTON", "Back", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 200, 60, 150, 30, hwnd, (HMENU)BTN_RESET, NULL, NULL);
}

void ShowReorganizeInterface(HWND hwnd) {
    ClearChildWindows(hwnd);

    // Reorganize interface
    CreateWindow("STATIC", "Click below to reorganize student data:", WS_VISIBLE | WS_CHILD,
                 20, 20, 300, 20, hwnd, NULL, NULL, NULL);

    CreateWindow("BUTTON", "Reorganize", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 20, 60, 150, 30, hwnd, (HMENU)BTN_SUBMIT_Reorganize, NULL, NULL);
    CreateWindow("BUTTON", "Back", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 200, 60, 150, 30, hwnd, (HMENU)BTN_RESET, NULL, NULL);
}

// Hidouchi Diaa takes calculer moyen an handleadd students
void calculer_moyenne(Etudiant *etudiant) {
    float somme_notes = 0;
    int somme_coefficients = 0;

    for (int i = 0; i < MAX_NOTES; i++) {
        somme_notes += etudiant->notes[i] * coefficients[i];
        somme_coefficients += coefficients[i];
    }

    etudiant->moyenne = somme_notes / somme_coefficients;
}
// Hidouchi Diaa takes calculer moyen an handleadd students
void HandleAddStudent(HWND hwnd) {
    Etudiant etudiant;
    char buffer[256];

    // Get the full name
    GetWindowText(hInputFields[0], buffer, sizeof(buffer));
    strncpy(etudiant.full_name, buffer, MAX_NAME - 1);
    etudiant.full_name[MAX_NAME - 1] = '\0';

    // Get the birth year
    GetWindowText(hInputFields[1], buffer, sizeof(buffer));
    etudiant.annee_naissance = atoi(buffer);

    // Get the matricule
    GetWindowText(hInputFields[2], buffer, sizeof(buffer));
    etudiant.numero_inscription = atoi(buffer);

    // Get the class
    GetWindowText(hInputFields[3], buffer, sizeof(buffer));
    strncpy(etudiant.classe, buffer, MAX_CLASSE - 1);
    etudiant.classe[MAX_CLASSE - 1] = '\0';

    // Get the grades
    for (int i = 0; i < MAX_NOTES; i++) {
        GetWindowText(hInputFields[4 + i], buffer, sizeof(buffer));
        etudiant.notes[i] = atof(buffer);
    }

    // Calculate the average
    calculer_moyenne(&etudiant);
    etudiant.supprime = 0;

    FILE *file = fopen(FILENAME, "a");
    if (file) {
        // Save the student data to the file
        fprintf(file, "%d,%s,%d,%s", etudiant.numero_inscription, etudiant.full_name, etudiant.annee_naissance, etudiant.classe);
        for (int i = 0; i < MAX_NOTES; i++) {
            fprintf(file, ",%.2f", etudiant.notes[i]);
        }
        fprintf(file, ",%.2f,%d\n", etudiant.moyenne, etudiant.supprime);
        fclose(file);
        MessageBox(hwnd, "Student added successfully!", "Success", MB_OK | MB_ICONINFORMATION);
        ResetFields(hwnd);
    } else {
        MessageBox(hwnd, "Error saving student data!", "Error", MB_OK | MB_ICONERROR);
    }
}
Etudiant searchedStudent;
// Benrahmoune aness takes handlesearchstudent fonction
void HandleSearchStudent(HWND hwnd) {
    char buffer[256];
    GetWindowText(hInputFields[0], buffer, sizeof(buffer));
    int numero_inscription = atoi(buffer);

    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        MessageBox(hwnd, "Error opening file!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    int found = 0;
    while (fscanf(file, "%d,%49[^,],%d,%9[^,],%f,%f,%f,%f,%f,%d\n",
                  &searchedStudent.numero_inscription, searchedStudent.full_name,
                  &searchedStudent.annee_naissance, searchedStudent.classe,
                  &searchedStudent.notes[0], &searchedStudent.notes[1], &searchedStudent.notes[2], &searchedStudent.notes[3],
                  &searchedStudent.moyenne, &searchedStudent.supprime) == 10) {

        if (searchedStudent.numero_inscription == numero_inscription && searchedStudent.supprime == 0) {
            found = 1;
            break;
        }
                  }

    fclose(file);

    if (found) {
        MessageBox(hwnd, "Student found!", "Success", MB_OK | MB_ICONINFORMATION);
    } else {
        MessageBox(hwnd, "Student not found or deleted!", "Not Found", MB_OK | MB_ICONWARNING);
    }
}
// boukahel zahrou takes HandleDeleteStudent
void HandleDeleteStudent(HWND hwnd) {
    char buffer[256];
    GetWindowText(hInputFields[0], buffer, sizeof(buffer));
    int numero_inscription = atoi(buffer);

    FILE *file = fopen(FILENAME, "r+"); // Open in read-write mode
    if (!file) {
        MessageBox(hwnd, "Error opening file!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    Etudiant etudiant;
    int found = 0;
    long pos;

    // Read through the file to find the student
    while (fscanf(file, "%d,%49[^,],%d,%9[^,],%f,%f,%f,%f,%f,%d\n",
                  &etudiant.numero_inscription, etudiant.full_name,
                  &etudiant.annee_naissance, etudiant.classe,
                  &etudiant.notes[0], &etudiant.notes[1], &etudiant.notes[2], &etudiant.notes[3],
                  &etudiant.moyenne, &etudiant.supprime) == 10) {

        if (etudiant.numero_inscription == numero_inscription && etudiant.supprime == 0) {
            // Calculate the position of the "supprime" field in the file
            pos = ftell(file) - 3; // Go back to the boolean field position (2 digits and comma)

            // Move the file pointer to the boolean field
            fseek(file, pos, SEEK_SET);

            // Update the deletion status (logical value 0 to 1)
            fprintf(file, "1");

            fflush(file); // Ensure changes are written to the file
            found = 1;
            break;
        }
                  }

    fclose(file);

    if (found) {
        MessageBox(hwnd, "Student deleted successfully!", "Success", MB_OK | MB_ICONINFORMATION);
    } else {
        MessageBox(hwnd, "Student not found!", "Not Found", MB_OK | MB_ICONWARNING);
    }
}


// kaddour abdelmalek takes HandleReorganize(HWND hwnd)
void HandleReorganize(HWND hwnd) {
    FILE *file = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    FILE *deletedFile = fopen("deletedstudents.txt", "a"); // Append mode for deleted students

    if (!file || !temp || !deletedFile) {
        MessageBox(hwnd, "Error opening files!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    Etudiant etudiant;
    int activeCount = 0, deletedCount = 0;

    while (fscanf(file, "%d,%49[^,],%d,%9[^,],%f,%f,%f,%f,%f,%d\n",
                  &etudiant.numero_inscription, etudiant.full_name,
                  &etudiant.annee_naissance, etudiant.classe,
                  &etudiant.notes[0], &etudiant.notes[1], &etudiant.notes[2], &etudiant.notes[3],
                  &etudiant.moyenne, &etudiant.supprime) == 10) {

        if (!etudiant.supprime) {
            // Write active students to the temp file
            fprintf(temp, "%d,%s,%d,%s,%.2f,%.2f,%.2f,%.2f,%.2f,%d\n",
                    etudiant.numero_inscription, etudiant.full_name,
                    etudiant.annee_naissance, etudiant.classe,
                    etudiant.notes[0], etudiant.notes[1], etudiant.notes[2], etudiant.notes[3],
                    etudiant.moyenne, etudiant.supprime);
            activeCount++;
        } else {
            // Write logically deleted students to deletedstudents.txt
            fprintf(deletedFile, "%d,%s,%d,%s,%.2f,%.2f,%.2f,%.2f,%.2f,%d\n",
                    etudiant.numero_inscription, etudiant.full_name,
                    etudiant.annee_naissance, etudiant.classe,
                    etudiant.notes[0], etudiant.notes[1], etudiant.notes[2], etudiant.notes[3],
                    etudiant.moyenne, etudiant.supprime);
            deletedCount++;
        }
    }

    fclose(file);
    fclose(temp);
    fclose(deletedFile);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    char message[150];
    sprintf(message, "File reorganized successfully!\nActive students: %d\nDeleted students moved: %d", activeCount, deletedCount);
    MessageBox(hwnd, message, "Success", MB_OK | MB_ICONINFORMATION);
}


long record_position = -1;
// falek wail HandleSubmitModifyStudent modify and HandleModifyStudent
void HandleModifyStudent(HWND hwnd) {
    char buffer[256];
    GetWindowText(hInputFields[0], buffer, sizeof(buffer));
    int numero_inscription = atoi(buffer);

    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        MessageBox(hwnd, "Error opening file!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    Etudiant etudiant;
    int found = 0;
    long record_position;

    while (!found && !feof(file)) {
        record_position = ftell(file); // Capture the position of the start of the current line
        if (fscanf(file, "%d,%49[^,],%d,%9[^,],%f,%f,%f,%f,%f,%d\n",
                   &etudiant.numero_inscription, etudiant.full_name,
                   &etudiant.annee_naissance, etudiant.classe,
                   &etudiant.notes[0], &etudiant.notes[1], &etudiant.notes[2],
                   &etudiant.notes[3], &etudiant.moyenne, &etudiant.supprime) == 10) {
            if (etudiant.numero_inscription == numero_inscription && !etudiant.supprime) {
                found = 1;
                break;
            }
                   }
    }

    fclose(file);

    if (found) {
        sprintf(buffer, "%d", etudiant.numero_inscription);
        SetWindowText(hInputFields[0], buffer);
        SetWindowText(hInputFields[3], buffer);
        SetWindowText(hInputFields[1], etudiant.full_name);
        sprintf(buffer, "%d", etudiant.annee_naissance);
        SetWindowText(hInputFields[2], buffer);
        SetWindowText(hInputFields[4], etudiant.classe);
        for (int i = 0; i < MAX_NOTES; i++) {
            sprintf(buffer, "%.2f", etudiant.notes[i]);
            SetWindowText(hInputFields[5 + i], buffer);
        }

        MessageBox(hwnd, "Student details fetched successfully!", "Success", MB_OK | MB_ICONINFORMATION);

        // Store the position in the global variable
        global_record_position = record_position;
    } else {
        MessageBox(hwnd, "Student not found!", "Not Found", MB_OK | MB_ICONWARNING);
    }
}

// falek wail HandleSubmitModifyStudent modify and HandleModifyStudent
void HandleSubmitModifyStudent(HWND hwnd) {
    // Ensure `global_record_position` is valid
    if (global_record_position == -1) {
        MessageBox(hwnd, "No record selected for modification!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    Etudiant etudiant;
    char buffer[256];

    // Get student details from input fields
    GetWindowText(hInputFields[1], buffer, sizeof(buffer));
    strncpy(etudiant.full_name, buffer, MAX_NAME - 1);
    etudiant.full_name[MAX_NAME - 1] = '\0';

    GetWindowText(hInputFields[2], buffer, sizeof(buffer));
    etudiant.annee_naissance = atoi(buffer);

    GetWindowText(hInputFields[3], buffer, sizeof(buffer));
    etudiant.numero_inscription = atoi(buffer);

    GetWindowText(hInputFields[4], buffer, sizeof(buffer));
    strncpy(etudiant.classe, buffer, MAX_CLASSE - 1);
    etudiant.classe[MAX_CLASSE - 1] = '\0';

    for (int i = 0; i < MAX_NOTES; i++) {
        GetWindowText(hInputFields[5 + i], buffer, sizeof(buffer));
        etudiant.notes[i] = atof(buffer);
    }

    calculer_moyenne(&etudiant);
    etudiant.supprime = 0;

    // Open the file for updating
    FILE *file = fopen(FILENAME, "r+");
    if (!file) {
        MessageBox(hwnd, "Error updating student data!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Move to the correct position
    fseek(file, global_record_position, SEEK_SET);

    // Overwrite the old line with spaces to prevent residual data
    memset(buffer, ' ', sizeof(buffer)); // Fill buffer with spaces
    buffer[sizeof(buffer) - 1] = '\0';   // Null-terminate
    fwrite(buffer, sizeof(char), strlen(buffer), file);

    // Return to the record position for writing the new record
    fseek(file, global_record_position, SEEK_SET);

    // Write the updated record
    fprintf(file, "%d,%s,%d,%s", etudiant.numero_inscription, etudiant.full_name,
            etudiant.annee_naissance, etudiant.classe);
    for (int i = 0; i < MAX_NOTES; i++) {
        fprintf(file, ",%.2f", etudiant.notes[i]);
    }
    fprintf(file, ",%.2f,%d", etudiant.moyenne, etudiant.supprime);

    fclose(file);

    MessageBox(hwnd, "Student details updated successfully!", "Success", MB_OK | MB_ICONINFORMATION);
}
 // djassem aissaoua takes  HandleExtractByClass


void HandleExtractByClass(HWND hwnd) {
    char buffer[256];
    GetWindowText(hInputFields[0], buffer, sizeof(buffer));
    char class_name[MAX_CLASSE];
    strncpy(class_name, buffer, MAX_CLASSE - 1);
    class_name[MAX_CLASSE - 1] = '\0';

    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        MessageBox(hwnd, "Error opening main file!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    Etudiant etudiants[100];
    int count = 0;

    // Read students from the file
    while (fscanf(file, "%d,%49[^,],%d,%9[^,],%f,%f,%f,%f,%f,%d\n",
                  &etudiants[count].numero_inscription, etudiants[count].full_name,
                  &etudiants[count].annee_naissance, etudiants[count].classe,
                  &etudiants[count].notes[0], &etudiants[count].notes[1], &etudiants[count].notes[2], &etudiants[count].notes[3],
                  &etudiants[count].moyenne, &etudiants[count].supprime) == 10) {

        // Filter by class and ensure the student is not logically deleted
        if (strcmp(etudiants[count].classe, class_name) == 0 && !etudiants[count].supprime) {
            count++;
        }
    }
    fclose(file);

    if (count == 0) {
        MessageBox(hwnd, "No students found in this class!", "Not Found", MB_OK | MB_ICONWARNING);
        return;
    }

    // Sort students by average in descending order
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (etudiants[i].moyenne < etudiants[j].moyenne) {
                Etudiant temp = etudiants[i];
                etudiants[i] = etudiants[j];
                etudiants[j] = temp;
            }
        }
    }

    // Write sorted students to a new file
    FILE *extractedFile = fopen("extractedstudents.txt", "w");
    if (!extractedFile) {
        MessageBox(hwnd, "Error creating extractedstudents.txt!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(extractedFile, "%d,%s,%d,%s,%.2f,%.2f,%.2f,%.2f,%.2f,%d\n",
                etudiants[i].numero_inscription, etudiants[i].full_name,
                etudiants[i].annee_naissance, etudiants[i].classe,
                etudiants[i].notes[0], etudiants[i].notes[1], etudiants[i].notes[2], etudiants[i].notes[3],
                etudiants[i].moyenne, etudiants[i].supprime);
    }
    fclose(extractedFile);

    char message[256];
    sprintf(message, "Students from class '%s' extracted successfully!\nCheck extractedstudents.txt for details.", class_name);
    MessageBox(hwnd, message, "Success", MB_OK | MB_ICONINFORMATION);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            ShowMainMenu(hwnd);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case BTN_ADD:
                    ShowAddStudentInterface(hwnd);
                    break;
                case BTN_SEARCH:
                    ShowSearchStudentInterface(hwnd);
                    break;
                case BTN_MODIFY:
                    ShowModifyStudentInterface(hwnd);
                    break;
                case BTN_DELETE:
                    ShowDeleteStudentInterface(hwnd);
                    break;
                case BTN_EXTRACT:
                    ShowExtractByClassInterface(hwnd);
                    break;
                case BTN_EXTRACT_SUBMIT:
                    HandleExtractByClass(hwnd);
                break;
                case BTN_SUBMIT_DELETE:
                    HandleDeleteStudent(hwnd);
                    break;
                case BTN_REORGANIZE:
                    ShowReorganizeInterface(hwnd);
                    break;
                case BTN_EXIT:
                    PostQuitMessage(0);
                    break;
                case BTN_RESET:
                    ShowMainMenu(hwnd);
                    break;
                case BTN_SUBMIT_ADD:
                    HandleAddStudent(hwnd);
                    break;
                case BTN_SUBMIT_SEARCH:
                    HandleSearchStudent(hwnd);
                    HandleModifyStudent(hwnd);
                    break;
                case BTN_SUBMIT_SEARCH2:
                    HandleSearchStudent(hwnd);
                    SHOWSEARCHSTUDENTRESULTINTERFACE(hwnd);
                break;
                case BTN_SUBMIT_MODIFY:
                    HandleSubmitModifyStudent(hwnd);
                    break;
                case BTN_SUBMIT_Reorganize:
                    HandleReorganize(hwnd);
                   break;
                case BTN_SUBMIT:
                    if (GetDlgCtrlID((HWND)lParam) == BTN_EXTRACT) {
                        HandleExtractByClass(hwnd);
                    } else if (GetDlgCtrlID((HWND)lParam) == BTN_REORGANIZE) {
                        HandleReorganize(hwnd);
                    }
                    break;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
/*
1,Ali Ahmed,2001,3A,15.50,12.00,14.00,16.50,14.20,0
2,Moulay Sarah,2000,2B,18.00,16.50,17.00,19.00,17.40,0
3,Zouaoui Sarah,1999,3A,13.00,11.00,15.50,14.00,13.50,0
4,Benmoussa Karim,2002,1C,10.00,9.50,12.00,11.00,10.80,0
5,Cherif Yasmine,2003,2B,19.00,18.50,17.50,20.00,18.80,0
6,Haddad Amine,2001,3A,14.00,13.00,15.00,16.00,14.80,1
7,Mezouar Fatima,2002,1A,16.00,15.00,14.50,18.00,15.80,0
8,Khellaf Mourad,2000,2B,12.00,10.50,11.00,13.00,12.10,0
9,Saidi Lila,2003,1C,8.00,9.00,7.50,10.00,8.70,0
10,Djebbar Hamid,2002,3A,20.00,19.50,18.00,17.00,18.70,0
11,Belarbi Malek,1998,3B,15.00,14.00,12.50,13.00,14.10,1
12,Guerfi Yacine,2000,2A,17.00,18.00,16.50,19.00,17.80,0
13,Amrani Samira,2001,3A,13.50,12.00,11.00,14.00,12.90,1
14,Rahmouni Nadia,1999,1B,9.00,8.50,10.00,9.00,8.90,0
15,Kaci,Lina 2003,2B,20.00,19.50,18.50,20.00,19.60,0
16,Boukhelfa Adam,2002,1C,11.00,10.50,12.00,10.00,10.80,1
17,Saadallah Amina,2001,3B,14.00,13.50,14.00,15.00,14.20,0
18,Hamdi Khaled,2002,2A,18.00,17.00,16.00,19.00,17.40,0
19,Belhadi Amel,1998,1A,10.00,9.50,8.00,11.00,9.80,0
20,Larbi Nassim,2001,2B,12.50,13.00,14.00,15.00,13.50,0
21,Aouini Salima,2003,3A,16.50,17.00,15.00,18.50,17.20,0
22,Chikhi Reda,1999,1B,10.50,9.00,8.00,12.00,10.30,1
23,Medjahdi Farid,2002,1C,14.00,13.50,14.00,15.00,14.30,0
24,Dib Leila,2003,3B,19.00,18.50,17.00,20.00,18.60,0
25,Nezzar Fouad,2001,2A,15.00,14.50,13.00,14.00,14.30,1
26,Belaid Amine,1999,2B,10.00,11.00,12.00,13.00,11.90,0
27,Cherrad Wafa,2000,1A,18.00,19.00,17.50,20.00,18.70,0
28,Fekir Imane,2001,2C,13.00,14.00,12.50,15.00,13.80,0
29,Khir Redouane,2003,3A,17.00,16.00,15.50,18.00,16.90,0
30,Selmani Hiba,1998,1B,9.00,8.50,10.00,9.50,9.10,1
31,Abdi Oussama,2002,2A,14.00,13.00,12.50,15.00,13.80,0
32,Ziani Malika,1999,3B,19.50,20.00,18.50,19.00,19.10,0
33,Zerouki Tarek,2003,1C,11.00,10.00,9.50,12.00,10.90,0
34,Mekki Soraya,2001,2B,15.00,14.50,13.00,16.00,14.60,1
35,Yahiaoui Khalil,2000,3A,13.50,12.50,14.00,15.00,13.90,0
36,Ramdani Yasmine,2002,1B,10.00,9.00,11.50,12.00,10.80,0
37,Tahar Hamza,2001,2A,18.50,17.50,19.00,20.00,18.80,0
38,Mokhtar Aya,2003,3B,14.50,15.00,16.00,14.00,14.80,0
39,Nour Lounes,2002,1A,12.00,13.00,14.50,11.50,12.90,0
40,Bendebka Samir,2000,2C,15.00,14.50,13.00,16.00,14.60,0

*/
