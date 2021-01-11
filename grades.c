#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked-list.h"
#include "grades.h"


/* grades struct */
struct grades {
	struct list *students;
};

/* student node element */
typedef struct student_data_ {
	struct list *courses;
	char *name;
	float gpa;
	int id;
	int num_courses;
} student_data;

/* course node element */
typedef struct course_data_ {
	char *name;
	int grade;
} course_data;

/**
 * @brief Clones student data from element to allocated memory pointed by output
 * @param element: void pointer to element to clone
 * @param output: pointer to pointer to cloned element
 * @return 0 if succes, -1 otherwise
 */
int student_data_clone(void *element, void **output) {

	student_data *elem = (student_data*)element;

	student_data *clone = (student_data*) malloc(sizeof(student_data));
    clone->name = (char*) malloc((strlen(elem->name)+1) * sizeof(char));
    
    /* Check mallocs success */
    if (clone == NULL || clone->name == NULL) {
        return -1;
    }

    /* Copy element fields */
    clone->courses = elem->courses;
    strcpy(clone->name, elem->name);
    clone->gpa = elem->gpa;
    clone->id = elem->id;
    clone->num_courses = elem->num_courses;  

	*output = (void*) clone;

	return 0;
}

/**
 * @brief Frees all memory allocated for student_data struct, including courses
 * @param element: pointer to element to destroy
 * @note Always succeeds
 */
void student_data_destroy(void *element) {
	student_data *elem = (student_data*)element;
	list_destroy(elem->courses);
	free(elem->name);
	free(elem);
}

/**
 * @brief Clones course data from element to allocated memory pointed by output
 * @param element: void pointer to element to clone
 * @param output: pointer to pointer to cloned element
 * @return 0 if succes, -1 otherwise
 */
int course_data_clone(void *element, void **output) {
	
	course_data *elem = (course_data*)element;

	course_data *clone = (course_data*) malloc(sizeof(course_data));
	clone->name = (char*) malloc((strlen(elem->name)+1) * sizeof(char));
    
    /* Check mallocs success */
    if (clone == NULL || clone->name == NULL) {
        return -1;
    }    

    /* Copy element fields */
    clone->grade = elem->grade;
    strcpy(clone->name, elem->name);

	*output = (void*) clone;

	return 0;
}

/**
 * @brief Frees all memory allocated for course_data struct
 * @param element: pointer to element to destroy
 * @note Always succeeds
 */
void course_data_destroy(void *element) {
	course_data *elem = (course_data*)element;
	free(elem->name);
	free(elem);
}

/**
 * @brief Prints student's name, id and course+grade list in format
 * @param cur_student_data: pointer to struct containing student's data
 * @return 0 if succeeds, -1 if any of the called functions fail
 * @note Format is:
 * STUDENT-NAME STUDENT-ID: COURSE-1-NAME COURSE-1-GRADE, [...] 
 */
int print_student_info(student_data *cur_student_data) {

	printf("%s %d:", cur_student_data->name, cur_student_data->id);

	if (list_size(cur_student_data->courses) == 0) {
		return 0;
	}

	struct node *cur_course = list_begin(cur_student_data->courses);
	course_data *cur_course_data;

	if (cur_course == NULL) {
		return -1;
	}

	// Move on the student's courses and print them
	while (cur_course) {
		cur_course_data = (course_data*)list_get(cur_course);

		if (cur_course_data == NULL) {
		return -1;
		}

		printf(" %s %d", cur_course_data->name, cur_course_data->grade);
		/* Check if there are more courses to print and print "," accordingly */
		if (list_next(cur_course)) {
			printf(",");
		}
		cur_course = list_next(cur_course);
	}
	// If we reached here the print was a success
	return 0;
}

struct grades* grades_init() {
	
	struct grades *grades = (struct grades*) malloc(sizeof(struct grades));

    grades->students = list_init(student_data_clone, student_data_destroy); 

    if (grades->students == NULL) {
    	return NULL;
    } else {
    	return grades;
    }                    
}

void grades_destroy(struct grades *grades) {
	list_destroy(grades->students);
	free(grades);
}

int grades_add_student(struct grades *grades, const char *name, int id) {

	/* --- Check for failures --- */
	if (grades == NULL) {
		return -1;
	}

	struct node *cur_student = list_begin(grades->students);
	student_data *cur_student_data;

	if (cur_student == NULL && list_size(grades->students) != 0) {
		return -1;
	}

	/* Check if student with id already exists */
	while (cur_student) {
		/* get student element and check if valid */
		cur_student_data = (student_data*) list_get(cur_student);
		if (cur_student_data == NULL) {
			return -1;
		}

		if (cur_student_data->id == id) {
			return -1;
		}

		cur_student = list_next(cur_student); 
	}

	/* --- Add student --- */
	student_data new_student;
	new_student.name = (char*) malloc((strlen(name)+1) * sizeof(char));
    
    /* Check malloc success */
    if (new_student.name == NULL) {
        return -1;
    }

    new_student.courses = list_init(course_data_clone, course_data_destroy);
    if (new_student.courses == NULL) {
    	free(new_student.name);
    	return -1;
    }

    strcpy(new_student.name, name);
    new_student.gpa = 0;
    new_student.id = id;
    new_student.num_courses = 0;
    
	if (list_push_back(grades->students, (void*)&new_student) != 0) {
		free(new_student.name);
    	return -1;
	}
	free(new_student.name);
	return 0;
}

int grades_add_grade(struct grades *grades,
                     const char *name,
                     int id,
                     int grade) {

	if (grade < 0 || grade > 100 || grades == NULL) {
		return -1;
	}

	struct node *cur_student = list_begin(grades->students);
	student_data *cur_student_data;

	if (cur_student == NULL && list_size(grades->students) != 0) {
		return -1;
	}

	while (cur_student) {

		/* get student element and check if valid */
		cur_student_data = (student_data*) list_get(cur_student);
		if (cur_student_data == NULL) {
			return -1;
		}

		if (cur_student_data->id == id) {

			struct node *cur_course = list_begin(cur_student_data->courses);
			course_data *cur_course_data;

			if (cur_course == NULL && list_size(cur_student_data->courses)!=0){
				return -1;
			}

			/* check if the student already has a course with "name" */
			while (cur_course) {

				/* get course element and check if valid */
				cur_course_data = (course_data*) list_get(cur_course);
				if (cur_course_data == NULL) {
					return -1;
				}

				if (!strcmp(cur_course_data->name, name)) {
					return -1;
				}

				cur_course = list_next(cur_course);
			}

			/* calculate new GPA and update relevant student data */
			cur_student_data->num_courses += 1;
			cur_student_data->gpa = (1/(float)cur_student_data->num_courses) * 
									(cur_student_data->gpa * 
									(cur_student_data->num_courses-1)
									+ (float) grade);

			/* add new course to student's course list */	
			course_data new_course;
			new_course.name = (char*) malloc((strlen(name)+1) * sizeof(char));
    
		    /* Check malloc success */
		    if (new_course.name == NULL) {
		        return -1;
		    }

			strcpy(new_course.name, name);
			new_course.grade = grade;

			if(list_push_back(cur_student_data->courses,(void*)&new_course)!=0){                              
				free(new_course.name);
		    	return -1;
			}
			free(new_course.name);
			return 0;
		}

		cur_student = list_next(cur_student);
	}

	/* if reached here, a student with "id" does not exist in "grades" */
	return -1;
}

float grades_calc_avg(struct grades *grades, int id, char **out) {

	if (grades == NULL) {
		*out = NULL;
		return -1;
	}

	struct node *cur_student = list_begin(grades->students);
	student_data *cur_student_data;

	while (cur_student) {

		/* get element and check if valid */
		cur_student_data = (student_data*)list_get(cur_student);
		if (cur_student_data == NULL) {
			return -1;
		}

		if (cur_student_data->id == id) {

			*out = (char*) malloc((strlen(cur_student_data->name)+1) * 
																  sizeof(char));
			if (*out == NULL) {
				return -1;
			}

			strcpy(*out, cur_student_data->name);
			return cur_student_data->gpa;

		}

		cur_student = list_next(cur_student);
	}
	
	/* if reached here, a student with "id" does not exist in "grades" */
	*out = NULL;
	return -1;
}

int grades_print_student(struct grades *grades, int id) {

	if (grades == NULL) {
		return -1;
	}

	if (list_size(grades->students) == 0) {
		return -1;
	}

	struct node *cur_student = list_begin(grades->students);
	student_data *cur_student_data;

	if (cur_student == NULL) {
		return -1;
	}

	while (cur_student) {

		/* get element and check if valid */
		cur_student_data = (student_data*) list_get(cur_student);
		if (cur_student_data == NULL) {
			return -1;
		}

		if (cur_student_data->id == id) {
			
			if (print_student_info(cur_student_data) != 0) {
				return -1;
			}
			printf("\n");

			return 0;

		}

		cur_student = list_next(cur_student);
	}
	/* if reached here, a student with "id" does not exist in "grades" */
	return -1;
}

int grades_print_all(struct grades *grades) {

	if (grades == NULL) {
		return -1;
	}

	if (list_size(grades->students) == 0) {
		return 0;
	}

	struct node *cur_student = list_begin(grades->students);
	student_data *cur_student_data;

	if (cur_student == NULL) {
		return -1;
	}

	while (cur_student) {

		/* get element and check if valid */
		cur_student_data = (student_data*) list_get(cur_student);
		if (cur_student_data == NULL) {
			return -1;
		}

		if (print_student_info(cur_student_data) != 0) {
			return -1;
		}
		printf("\n");
		
		cur_student = list_next(cur_student);
	}

	return 0;
}