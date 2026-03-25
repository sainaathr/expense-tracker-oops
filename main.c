#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

// WebAssembly specific Asynchronous Input Interface
EM_ASYNC_JS(void, get_string_async_js, (char* buffer_ptr, int max_len), {
    await new Promise(resolve => {
        let term = document.getElementById("output");
        let inputContainer = document.getElementById("input-container");
        let inputField = document.getElementById("cmd-input");
        
        inputContainer.style.display = "flex";
        inputField.focus();
        
        let handleEnter = function(e) {
            if (e.key === "Enter") {
                inputField.removeEventListener("keydown", handleEnter);
                let val = inputField.value.trim();
                inputField.value = "";
                inputContainer.style.display = "none";
                
                term.innerHTML += '<span style="color: #66fcf1; font-weight: bold;">> ' + (val === "" ? " " : val) + "</span><br>";
                term.scrollTop = term.scrollHeight;
                
                stringToUTF8(val, buffer_ptr, max_len);
                resolve();
            }
        };
        inputField.addEventListener("keydown", handleEnter);
    });
});

void get_string_input(char* buffer, int max_len) {
    fflush(stdout); // Crucial: Flushes text to the screen BEFORE pausing for input
    get_string_async_js(buffer, max_len);
}

int get_int_input() {
    char buf[50];
    get_string_input(buf, 50);
    return atoi(buf);
}

float get_float_input() {
    char buf[50];
    get_string_input(buf, 50);
    return atof(buf);
}

#else

// Native Desktop Standard C Input Interface
void get_string_input(char* buffer, int max_len) {
    fflush(stdout);
    if(fgets(buffer, max_len, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0; 
    }
}

int get_int_input() {
    int val = 0;
    char buf[50];
    fflush(stdout);
    if(fgets(buf, 50, stdin) != NULL) {
        sscanf(buf, "%d", &val);
    }
    return val;
}

float get_float_input() {
    float val = 0.0f;
    char buf[50];
    fflush(stdout);
    if(fgets(buf, 50, stdin) != NULL) {
        sscanf(buf, "%f", &val);
    }
    return val;
}

#endif

#define MAX_EXPENSES 1000
#define FILE_NAME "expenses.txt"

typedef struct {
    char date[15]; 
    char category[50];
    float amount;
    char note[100];
} Expense;

Expense expenses[MAX_EXPENSES];
int expense_count = 0;
float monthly_budget = 1000.0; 

void load_expenses() {
    FILE *file = fopen(FILE_NAME, "r");
    if (!file) return; 
    expense_count = 0;
    while (fscanf(file, "%14[^|]|%49[^|]|%f|%99[^\n]\n", 
            expenses[expense_count].date, 
            expenses[expense_count].category, 
            &expenses[expense_count].amount, 
            expenses[expense_count].note) == 4) {
        expense_count++;
        if (expense_count >= MAX_EXPENSES) break;
    }
    fclose(file);
}

void save_expenses() {
    FILE *file = fopen(FILE_NAME, "w");
    if (!file) {
        printf("Error: Could not open file to save expenses.\n");
        return;
    }
    for (int i = 0; i < expense_count; i++) {
        fprintf(file, "%s|%s|%.2f|%s\n", 
                expenses[i].date, 
                expenses[i].category, 
                expenses[i].amount, 
                expenses[i].note);
    }
    fclose(file);
}

void add_expense() {
    if (expense_count >= MAX_EXPENSES) {
        printf("Expense list is full! Cannot add more.\n");
        return;
    }
    
    Expense e;
    printf("\n--- Add Expense ---\n");
    printf("Enter Date (YYYY-MM-DD):\n");
    get_string_input(e.date, 15);
    
    printf("Enter Category (e.g., Food, Travel, Medical):\n");
    get_string_input(e.category, 50);
    
    printf("Enter Amount:\n");
    e.amount = get_float_input();
    
    printf("Enter Note:\n");
    get_string_input(e.note, 100);
    
    expenses[expense_count++] = e;
    save_expenses();
    printf("Expense added successfully!\n");
}

void view_expenses_by_month() {
    char month[10]; 
    printf("\nEnter Month (YYYY-MM):\n");
    get_string_input(month, 10);
    
    float total = 0;
    printf("\n--- Expenses for %s ---\n", month);
    printf("%-12s %-15s %-10s %-20s\n", "Date", "Category", "Amount", "Note");
    printf("--------------------------------------------------------------\n");
    
    for (int i = 0; i < expense_count; i++) {
        if (strncmp(expenses[i].date, month, 7) == 0) {
            printf("%-12s %-15s %-10.2f %-20s\n", 
                   expenses[i].date, expenses[i].category, expenses[i].amount, expenses[i].note);
            total += expenses[i].amount;
        }
    }
    printf("--------------------------------------------------------------\n");
    printf("Total Expense for %s: $%.2f\n", month, total);
    
    if (total > monthly_budget) {
        printf("\n*** WARNING: You have exceeded your monthly budget of $%.2f! ***\n", monthly_budget);
    } else {
        printf("Remaining Budget: $%.2f\n", monthly_budget - total);
    }
}

void view_category_totals() {
    printf("\n--- Category-wise Totals ---\n");
    char categories[MAX_EXPENSES][50];
    float totals[MAX_EXPENSES] = {0};
    int cat_count = 0;
    
    for (int i = 0; i < expense_count; i++) {
        int found = 0;
        for (int j = 0; j < cat_count; j++) {
            if (strcmp(expenses[i].category, categories[j]) == 0) {
                totals[j] += expenses[i].amount;
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(categories[cat_count], expenses[i].category);
            totals[cat_count] = expenses[i].amount;
            cat_count++;
        }
    }
    
    printf("%-20s %-10s\n", "Category", "Total Amount");
    printf("---------------------------------\n");
    for (int i = 0; i < cat_count; i++) {
        printf("%-20s $%.2f\n", categories[i], totals[i]);
    }
    if (cat_count == 0) {
        printf("No expenses recorded yet.\n");
    }
}

void search_expenses() {
    printf("\n--- Search Expenses ---\n");
    printf("1. By Date (YYYY-MM-DD)\n");
    printf("2. By Category\n");
    printf("Enter choice:\n");
    
    int choice = get_int_input();
    
    char query[50];
    if (choice == 1) {
        printf("Enter Date (YYYY-MM-DD):\n");
        get_string_input(query, 50);
    } else if (choice == 2) {
        printf("Enter Category:\n");
        get_string_input(query, 50);
    } else {
        printf("Invalid choice.\n");
        return;
    }
    
    printf("\n%-12s %-15s %-10s %-20s\n", "Date", "Category", "Amount", "Note");
    printf("--------------------------------------------------------------\n");
    int found = 0;
    for (int i = 0; i < expense_count; i++) {
        if ((choice == 1 && strcmp(expenses[i].date, query) == 0) ||
            (choice == 2 && strcmp(expenses[i].category, query) == 0)) {
            printf("%-12s %-15s %-10.2f %-20s\n", 
                   expenses[i].date, expenses[i].category, expenses[i].amount, expenses[i].note);
            found = 1;
        }
    }
    if (!found) {
        printf("No matching expenses found.\n");
    }
}

void set_budget() {
    printf("\nCurrent Monthly Budget limit: $%.2f\n", monthly_budget);
    printf("Enter new monthly budget:\n");
    float new_budget = get_float_input();
    
    if (new_budget >= 0) {
        monthly_budget = new_budget;
        printf("Budget updated successfully to $%.2f.\n", monthly_budget);
    } else {
        printf("Invalid budget entered.\n");
    }
}

int main() {
    load_expenses();
    
    int choice;
    while (1) {
        printf("\n=========================================\n");
        printf("      EXPENSE & BUDGET TRACKER\n");
        printf("=========================================\n");
        printf("1. Add Expense\n");
        printf("2. View Expenses by Month (Budget Check)\n");
        printf("3. Category-wise Totals\n");
        printf("4. Search Expenses\n");
        printf("5. Set Monthly Budget Limit\n");
        printf("6. Exit\n");
        printf("=========================================\n");
        printf("Enter your choice:\n");
        
        choice = get_int_input();
        
        switch (choice) {
            case 1: add_expense(); break;
            case 2: view_expenses_by_month(); break;
            case 3: view_category_totals(); break;
            case 4: search_expenses(); break;
            case 5: set_budget(); break;
            case 6: 
                printf("Exiting... Memory session cleared on tab close.\n");
                return 0;
            default:
                if (choice != 0) {
                    printf("Invalid choice. Please pick between 1-6.\n");
                }
        }
        
#ifdef __EMSCRIPTEN__
        emscripten_sleep(10);
#endif
    }
    return 0;
}