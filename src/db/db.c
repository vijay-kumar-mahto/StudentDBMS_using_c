#include "db.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Student* student_new(const char *reg_no, const char *name, const char *dept, const char *email, const char *mobile) {
    Student *student = g_new0(Student, 1);
    student->reg_no = g_strdup(reg_no);
    student->name = g_strdup(name);
    student->dept = g_strdup(dept);
    student->email = g_strdup(email ? email : "");
    student->mobile = g_strdup(mobile ? mobile : "");
    return student;
}

void student_free(Student *student) {
    if (!student) return;
    g_free(student->reg_no);
    g_free(student->name);
    g_free(student->dept);
    g_free(student->email);
    g_free(student->mobile);
    g_free(student);
}

sqlite3* db_init(void) {
    sqlite3 *db;
    int rc = sqlite3_open("data/students.db", &db);
    if (rc != SQLITE_OK) {
        g_printerr("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS students ("
                      "reg_no TEXT PRIMARY KEY, "
                      "name TEXT NOT NULL, "
                      "dept TEXT NOT NULL, "
                      "email TEXT, "
                      "mobile TEXT);";
    rc = sqlite3_exec(db, sql, 0, 0, NULL);
    if (rc != SQLITE_OK) {
        g_printerr("Failed to create table: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    return db;
}

int db_add_student(sqlite3 *db, Student *student) {
    const char *sql = "INSERT INTO students (reg_no, name, dept, email, mobile) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return 0;

    sqlite3_bind_text(stmt, 1, student->reg_no, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, student->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, student->dept, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, student->email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, student->mobile, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

int db_update_student(sqlite3 *db, Student *student) {
    const char *sql = "UPDATE students SET name = ?, dept = ?, email = ?, mobile = ? WHERE reg_no = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return 0;

    sqlite3_bind_text(stmt, 1, student->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, student->dept, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, student->email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, student->mobile, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, student->reg_no, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

int db_delete_student(sqlite3 *db, const char *reg_no) {
    const char *sql = "DELETE FROM students WHERE reg_no = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return 0;

    sqlite3_bind_text(stmt, 1, reg_no, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

GList* db_get_all_students(sqlite3 *db) {
    const char *sql = "SELECT reg_no, name, dept, email, mobile FROM students;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return NULL;

    GList *students = NULL;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *reg_no = (const char *)sqlite3_column_text(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        const char *dept = (const char *)sqlite3_column_text(stmt, 2);
        const char *email = (const char *)sqlite3_column_text(stmt, 3);
        const char *mobile = (const char *)sqlite3_column_text(stmt, 4);

        Student *student = student_new(reg_no, name, dept, email, mobile);
        students = g_list_append(students, student);
    }

    sqlite3_finalize(stmt);
    return students;
}

GHashTable* db_get_dept_stats(sqlite3 *db) {
    GHashTable *stats = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    const char *sql = "SELECT dept, COUNT(*) as count FROM students GROUP BY dept;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        g_hash_table_destroy(stats);
        return NULL;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *dept = (const char *)sqlite3_column_text(stmt, 0);
        int count = sqlite3_column_int(stmt, 1);
        g_hash_table_insert(stats, g_strdup(dept), g_memdup2(&count, sizeof(int)));
    }

    sqlite3_finalize(stmt);
    return stats;
}

void export_to_csv(sqlite3 *db) {
    GList *students = db_get_all_students(db);
    if (!students) {
        g_printerr("No students to export or failed to fetch students\n");
        return;
    }

    FILE *csv_file = fopen("data/students_export.csv", "w");
    if (!csv_file) {
        g_printerr("Failed to open students_export.csv for writing\n");
        g_list_free_full(students, (GDestroyNotify)student_free);
        return;
    }

    fprintf(csv_file, "Registration No.,Name,Department,Email,Mobile\n");

    for (GList *iter = students; iter != NULL; iter = iter->next) {
        Student *student = (Student *)iter->data;
        fprintf(csv_file, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n",
                student->reg_no, student->name, student->dept,
                student->email ? student->email : "",
                student->mobile ? student->mobile : "");
    }

    fclose(csv_file);
    g_list_free_full(students, (GDestroyNotify)student_free);
}

// Modified: Added function to delete all students from the database
int db_delete_all_students(sqlite3 *db) {
    const char *sql = "DELETE FROM students;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) return 0;

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}
