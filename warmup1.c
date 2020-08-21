/******************************************************************************/
/* Important CSCI 402 usage information:                                      */
/*                                                                            */
/* This fils is part of CSCI 402 programming assignments at USC.              */
/*         53616c7465645f5fd1e93dbf35cbffa3aef28f8c01d8cf2ffc51ef62b26a       */
/*         f9bda5a68e5ed8c972b17bab0f42e24b19daa7bd408305b1f7bd6c7208c1       */
/*         0e36230e913039b3046dd5fd0ba706a624d33dbaa4d6aab02c82fe09f561       */
/*         01b0fd977b0051f0b0ce0c69f7db857b1b5e007be2db6d42894bf93de848       */
/*         806d9152bd5715e9                                                   */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove or alter this comment block.          */
/* If this comment block is removed or altered in a submitted file, 20 points */
/*         will be deducted.                                                  */
/******************************************************************************/

/*
 * Author:      William Chia-Wei Cheng (bill.cheng@acm.org)
 *
 * @(#)$Id: warmup1.c,v 1.1 2019/05/19 23:34:12 cvsps Exp $
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#include "my402list.h"

/*Declaring a struct object datastructure to hold the fields*/
typedef struct transactionobj {
    char type;
    unsigned int timestamp;
    int amount;
    char description[1024];
    int line_number;
} Transactionobject;

void process_command(int argc, char * argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "Missing commandline arguments\nUsage information: warmup1 sort [tfile]\n");
        exit(-1);
    }
    else if(strcmp(argv[1], "sort"))
    {
        fprintf(stderr, "Incorrect command usage\nUsage information: warmup1 sort [tfile]\n");
        exit(-1);
    }
    else if(argc > 3)
    {
        fprintf(stderr, "Too many commandline arguments\nUsage information: warmup1 sort [tfile]\n");
        exit(-1);
    }
}

FILE * set_file_stream(int argc, char * argv[])
{
    FILE * file_ptr;
    if(argc == 3)
    {
        struct stat file_status;    
        stat(argv[2], &file_status);    /*Get file status*/
        if(S_ISDIR(file_status.st_mode))
        {
            fprintf(stderr, "Error: Input file %s is a directory!\n", argv[2]);
            exit(-1);
        }

        if((file_ptr = fopen(argv[2], "r")) == NULL)
        {
            fprintf(stderr, "Error with input file %s: ", argv[2]);
            perror("");
            exit(-1);
        }

        if(!S_ISREG(file_status.st_mode))
        {
            fprintf(stderr, "Error with input file: %s is not a regular file", argv[2]);
            exit(-1);
        }    
    }
    else
    {
        file_ptr = stdin;   /*Get input from standard input if number of arguments = 2*/
    }

    return file_ptr;
}

void change_timestamp(char *temp, unsigned int timestamp)
{
    time_t tt = timestamp;
    char *time_s = ctime(&tt);  /*Get timestamp in format WWW MMM DD HH:MM:SS YYYY*/
    strncpy(temp, time_s, strlen(time_s));
    memmove(&temp[11], &temp[20], strlen(temp) - 11);   /*Remove HH:MM:SS part from the timestamp*/
}

void change_description(char * desc)
{
    int desc_ptr = 0;
    while(desc[desc_ptr] != '\0')
    {
        if(desc_ptr > 23)
        {
            desc[24] = '\0';    /*Limit Description field to 24 characters long!*/
            break;
        }
        else if(desc[desc_ptr] == '\n')
        {
            break;
        }
        desc_ptr++;
    }
}

void change_amount(char * amount_string, int amount_value)
{
    char decimal_unit, decimal_ten;
    int amount_val = amount_value;
    char comma_amount[15], amount[11];

    decimal_unit = (amount_val % 10) + '0';     /*Contains 2nd digit after decimal point*/
    amount_val = amount_val / 10;
    decimal_ten = (amount_val % 10) + '0';      /*Contains 1st digit after decimal point*/
    amount_val = amount_val / 10;
    sprintf(amount, "%d", amount_val);

    /*Convert Number to Currency format*/
    if(strlen(amount) > 3)
    {
        int j = strlen(amount)-1, i = 0, count = 0;
        while(j > -1)
        {
            comma_amount[i] = amount[j];
            i++;
            j--;
            count++;
            if(count%3 == 0 && j != -1)
            {
                comma_amount[i] = ',';
                i++;
            }
        }
        comma_amount[i] = '\0';
        int com_ptr = strlen(comma_amount)-1, act_amt_ptr = 0;
        while(com_ptr > -1)
        {
            amount_string[act_amt_ptr++] = comma_amount[com_ptr--];
        }
        amount_string[act_amt_ptr++] = '.';
        amount_string[act_amt_ptr++] = decimal_ten;
        amount_string[act_amt_ptr++] = decimal_unit;
        amount_string[act_amt_ptr] = '\0';
    }
    else
    {
        int amt_2_ptr = 0;
        while(amt_2_ptr < strlen(amount))
        {
            amount_string[amt_2_ptr] = amount[amt_2_ptr];
            amt_2_ptr++;
        }
        amount_string[amt_2_ptr++] = '.';
        amount_string[amt_2_ptr++] = decimal_ten;
        amount_string[amt_2_ptr++] = decimal_unit;
        amount_string[amt_2_ptr] = '\0';
    }
}

void print_list(My402List *list_object)
{
    fprintf(stdout, "+-----------------+--------------------------+----------------+----------------+\n");
    fprintf(stdout, "|       Date      | Description              |         Amount |        Balance |\n");
    fprintf(stdout, "+-----------------+--------------------------+----------------+----------------+\n");

    long long balance_amount = 0;
    My402ListElem *current_ptr = NULL;
    for(current_ptr = My402ListFirst(list_object); current_ptr != NULL; current_ptr = My402ListNext(list_object, current_ptr))
    {
        Transactionobject *transactionobj = (Transactionobject *)(current_ptr -> obj);
        int withdrawal = 0, balance_flag = 0;

        if(transactionobj -> type == '-')
        {
            withdrawal = 1;
        }

        /*Change timestamp format*/
        char temp_time[30];
        change_timestamp(temp_time, transactionobj -> timestamp);
        char *c_ptr = temp_time;

        /*Change Description*/
        change_description(transactionobj -> description);
        
        /*Change amount*/
        char act_amount[20];
        change_amount(act_amount, transactionobj -> amount);
       
        /*Calculate balance*/
        if(withdrawal)
        {
            balance_amount = balance_amount - (transactionobj -> amount); 
        }
        else
        {
            balance_amount = balance_amount + (transactionobj -> amount);
        }

        if(balance_amount < 0)
        {
            balance_flag = 1;
        }

        char balance[20];
        long long check_bal = abs(balance_amount);
        check_bal = check_bal / 10;
        check_bal = check_bal / 10;
        if(check_bal > 10000000)
        {
            char str[] = "?,???,???.??";
            strncpy(balance, str, 13);
        }
        else
        {
            change_amount(balance, abs(balance_amount));   
        }

        if(withdrawal && balance_flag)
        {
            printf("| %s | %-24s | (%12s) | (%12s) |\n", strtok(c_ptr, "\n"), transactionobj -> description, act_amount, balance);
        }
        else if(withdrawal && !balance_flag)
        {
            printf("| %s | %-24s | (%12s) |  %12s  |\n", strtok(c_ptr, "\n"), transactionobj -> description, act_amount, balance);
        }
        else if(!withdrawal && balance_flag)
        {
            printf("| %s | %-24s |  %12s  | (%12s) |\n", strtok(c_ptr, "\n"), transactionobj -> description, act_amount, balance);
        }
        else
        {
            printf("| %s | %-24s |  %12s  |  %12s  |\n", strtok(c_ptr, "\n"), transactionobj -> description, act_amount, balance);  
        }
    }
    fprintf(stdout, "+-----------------+--------------------------+----------------+----------------+\n");
}

void insertion_sort(Transactionobject *new_transaction_obj, My402List *list_object)
{
    if(My402ListEmpty(list_object))
    {
        (void)My402ListAppend(list_object, (void *)new_transaction_obj);
    }
    else
    {
        My402ListElem *current_ptr = My402ListFirst(list_object);
        while(current_ptr)
        {
            Transactionobject *current_transaction_obj = (Transactionobject *)(current_ptr -> obj);
            if(current_transaction_obj -> timestamp < new_transaction_obj -> timestamp)
            {
                current_ptr = My402ListNext(list_object, current_ptr);
                if(!current_ptr)
                {
                    (void)My402ListAppend(list_object, (void *)new_transaction_obj);
                }
            }
            else if(current_transaction_obj -> timestamp > new_transaction_obj -> timestamp)
            {
                (void)My402ListInsertBefore(list_object, (void *)new_transaction_obj, current_ptr);
                break;
            }
            else
            {
                fprintf(stderr, "Error in line:%d ---> Duplicate Timestamp! Given timestamp already exists. Please change the timestamp.\n", new_transaction_obj -> line_number);
                exit(-1);
            }
        }   
    }
}

void process_input_line(char *input_line, int line_num, My402List *list_obj_ptr)
{
    if(input_line[1024] != '\0')
    {
        fprintf(stderr, "Invalid input at line:%d, input characters in the line exceed line limit(1024)!\n", line_num);
        exit(-1);
    }
    else
    {
        Transactionobject *transaction_obj = (Transactionobject *) malloc(sizeof(Transactionobject));

        /*Process Transaction Type*/
        int input_ptr = 0;
        char type = input_line[input_ptr++];
        if(type != '+' && type != '-')
        {
            fprintf(stderr, "Error at line:%d with field:Transaction Type ---> Not in the right format, transaction type undefined!\nUsage information for Field Transaction Type: Should contain either + or -\n", line_num);
            exit(-1);
        }
        else if(input_line[input_ptr] != '\t')
        {
            if(input_line[input_ptr] == '\0' || input_line[input_ptr] == '\n')
            {
                fprintf(stderr, "Error at line:%d Timestamp, Amount and Description fields are missing!\n", line_num);
                exit(-1);
            }
            else
            {
                fprintf(stderr, "Error at line:%d with field:Transaction Type ---> Not in the right format, contains too many characters!\nUsage information for Field Transaction Type: Should contain either + or -\n", line_num);
                exit(-1);
            }    
        }

        transaction_obj -> type = type;

        /*Process Transaction Timestamp*/
        input_ptr++;
        if(input_line[input_ptr] == '\0' || input_line[input_ptr] == '\n')
        {
            fprintf(stderr, "Error at line:%d Timestamp, Amount and Description fields are missing!\n", line_num);
            exit(-1);
        }
        else if(input_line[input_ptr] == '\t')
        {
            fprintf(stderr, "Error at line:%d Fields should be separated by only one <TAB> character!There are more than one after firld Transaction type!\n", line_num);
            exit(-1);
        }
        int max_time_length = 10, ch_1 = 0;
        char timestamp[max_time_length + 1];
        while(input_line[input_ptr] != '\t')
        {
            if(ch_1 >= max_time_length)
            {
                fprintf(stderr, "Error at line:%d with field:Transaction Timestamp ---> Not in the right format, field is too long!\nUsage information for Field Transaction timestamp: Should contain only numbers and length should not be greater than the length of largest unsigned integer 4,294,967,295\n", line_num);
                exit(-1);   
            }
            else if(input_line[input_ptr] == '\n')
            {
                fprintf(stderr, "Error at line:%d Amount and Description fields are missing\n", line_num);
                exit(-1);
            }
            else if(input_line[input_ptr] > '9' || input_line[input_ptr] < '0')
            {
                fprintf(stderr, "Error at line:%d with field:Transaction Timestamp ---> Not in the right format, field contains characters that are not numbers!\nUsage information for Field Transaction timestamp: Should contain only numbers and length should not be greater than the length of largest unsigned integer 4,294,967,295\n", line_num);
                exit(-1);
            }
            else
            {
                timestamp[ch_1++] = input_line[input_ptr++];
            }
        }
        
        timestamp[ch_1] = '\0';
  
        time_t current_timestamp = time(NULL);
       
        unsigned int input_timestamp = atoi(timestamp);
      
        if(input_timestamp > current_timestamp)
        {
            fprintf(stderr, "Error at line:%d with field:Transaction Timestamp ---> Invalid Timestamp, input timestamp exceeds the current timestamp!\n", line_num);
            exit(-1);
        }
        transaction_obj -> timestamp = input_timestamp;

        /*Process Transaction Amount*/
        input_ptr++;
        if(input_line[input_ptr] == '\0' || input_line[input_ptr] == '\n')
        {
            fprintf(stderr, "Error at line:%d Amount and Description fields are missing!\n", line_num);
            exit(-1);
        }
        else if(input_line[input_ptr] == '\t')
        {
            fprintf(stderr, "Error at line:%d Fields should be separated by only one <TAB> character!There are more than one after the firld Transaction timestamp!\n", line_num);
            exit(-1);
        }

        int max_amt_length = 10, ch_2 = 0, decimal_flag = 0;
        char amount[max_amt_length + 1];
        while(input_line[input_ptr] != '\t')
        {
            if(ch_2 > 7)
            {
                fprintf(stderr, "Error at line:%d with field:Transaction Amount ---> Not in the right format, field contains value more than 10 million!\n", line_num);
                exit(-1);
            }
            else if(input_line[input_ptr] == '\n' || input_line[input_ptr] == '\0')
            {
                fprintf(stderr, "Error at line:%d Description field is missing!\n", line_num);
                exit(-1);
            }
            else if(input_line[input_ptr] == '.')
            {
                decimal_flag = 1;
                break;
            }
            else if(input_line[input_ptr] <= '9' || input_line[input_ptr] >= '0')
            {
                amount[ch_2++] = input_line[input_ptr++];
            }
            else
            {
                fprintf(stderr, "Error at line:%d with field:Transaction Amount ---> Not in the right format, field contains that are not numbers!\nUsage information for Field Transaction amount: Should contain only numbers in the format xxxxxxx.xx\n", line_num);
                exit(-1);
            }
        }

        if(ch_2 == 0)
        {
            amount[ch_2++] = '0';
        }

        if(decimal_flag)
        {
            input_ptr++;
            if(input_line[input_ptr] == '\t')
            {
                amount[ch_2++] = '0';
                amount[ch_2++] = '0';
            }
            else if(input_line[input_ptr] == '\n')
            {
                fprintf(stderr, "Error at line:%d Description field is missing!\n", line_num);
                exit(-1);
            }
            int decimal_numbers = 2, dec_count = 0;
            while(input_line[input_ptr] != '\t')
            {
                if(input_line[input_ptr] == '\n'  || input_line[input_ptr] == '\0')
                {
                    fprintf(stderr, "Error at line:%d Description field is missing!\n", line_num);
                    exit(-1);
                }
                else if(dec_count >= decimal_numbers)
                {
                    fprintf(stderr, "Error at line:%d with field:Transaction Amount ---> Not in the right format, field contains more than two digits after decimal point.\n", line_num);
                    exit(-1);
                }
                else if(input_line[input_ptr] <= '9' || input_line[input_ptr] >= '0')
                {
                    amount[ch_2++] = input_line[input_ptr++];
                }
                else
                {
                    fprintf(stderr, "Error at line:%d with field:Transaction Amount ---> Not in the right format, field contains that are not numbers!\nUsage information for Field Transaction amount: Should contain only numbers in the format xxxxxxx.xx\n", line_num);
                    exit(-1);
                }
                dec_count++;
            }
        }
        else
        {
            amount[ch_2++] = '0';
            amount[ch_2++] = '0';
        }

        amount[ch_2] = '\0';
        int new_amt = atoi(amount);
        if(new_amt <= 0)
        {
            fprintf(stderr, "Error at line:%d with field:Transaction Amount ---> Value should be positive\n", line_num);
            exit(-1);
        }
        transaction_obj -> amount = new_amt;
        
        /*Process Transaction Description*/
        input_ptr++;
        if(input_line[input_ptr] == '\0')
        {
            fprintf(stderr, "Error at line:%d Description field is missing!\n", line_num);
            exit(-1);
        }

        while(input_line[input_ptr] == ' ')
        {
            input_ptr++;
        }

        if(input_line[input_ptr] == '\0' || input_line[input_ptr] == '\n')
        {
            fprintf(stderr, "Error at line:%d Description field is missing!\n", line_num);
            exit(-1);
        }

        int ch_3 = 0;

        while(input_line[input_ptr] != '\0')
        {
            if(input_line[input_ptr] == '\t')
            {
                fprintf(stderr, "Error at line:%d, Not in the right format! Line contains too many fields!\n", line_num);
                exit(-1);
            }
            else if(input_line[input_ptr] == '\n')
            {
                input_ptr++;
            }
            else if(input_line[input_ptr] >= 0 && input_line[input_ptr] <= 127)
            {
                transaction_obj -> description[ch_3++] = input_line[input_ptr++];
            }
        }

        transaction_obj -> description[ch_3] = '\0';

        transaction_obj -> line_number = line_num;

        insertion_sort(transaction_obj, list_obj_ptr);
    }
}

void read_input(FILE * fptr, My402List *list_obj_ptr)
{
    char buffer[1030]; //declaring size extra to consider input line length longer than 1024
    buffer[1024] = '\0';
    int line_no = 1;
    while(!feof(fptr))
    {
        if(fgets(buffer, 1030, fptr))
        {
            process_input_line(buffer, line_no, list_obj_ptr);
            line_no++;
        }
    }
}

int main(int argc, char *argv[])
{
    process_command(argc, argv);
    FILE * file = set_file_stream(argc, argv);
    My402List list_object;
    memset(&list_object, 0, sizeof(My402List));
    (void)My402ListInit(&list_object);
    read_input(file, &list_object);
    print_list(&list_object);
}

