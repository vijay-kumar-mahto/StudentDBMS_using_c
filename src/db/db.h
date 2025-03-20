#ifndef DB_H
#define DB_H

#include <sqlite3.h>
#include <gtk/gtk.h>

typedef struct {
    char *reg_no;
    char *name;
    char *dept;
    char *email;
    char *mobile;
} Student;

sqlite3* db_init(void);
Student* student_new(const char *reg_no, const char *name, const char *dept, const char *email, const char *mobile);
void student_free(Student *student);
int db_add_student(sqlite3 *db, Student *student);
int db_update_student(sqlite3 *db, Student *student);
int db_delete_student(sqlite3 *db, const char *reg_no);
GList* db_get_all_students(sqlite3 *db);
GHashTable* db_get_dept_stats(sqlite3 *db);
void export_to_csv(sqlite3 *db);

// Modified: Added declaration for deleting all students
int db_delete_all_students(sqlite3 *db);

#endif