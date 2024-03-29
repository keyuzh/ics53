#include "constants2.h"
#include "helpers2.h"
#include "hw2.h"

int main(int argc, char* argv[]) {
    errno = 4;
    search_t criterion = {NULL, NULL, 0, {0,0,0}, NULL};

	int I_flag = 0;
	int D_flag = 0;
	int G_flag = 0;
	int N_flag = 0;
	int K_flag = 0;
    int NUM_arg = 0;
    char ORDER_arg = 'n';   // default no order
    char* OUTFILE = NULL;

    // Use basic getopt to parse flags and respective arguments
    int c;
    while ((c = getopt(argc, argv, "I:D:G:N:K:Hn:ado:" )) >= 0) {
        switch (c) {
            case 'I':
            	I_flag += 1;
                if (!allDigits(optarg)) {
                    fprintf(stderr, USAGE_MSG);
                    exit(1);
                }
				criterion.ISBN = atoi(optarg);
				break;
            case 'D':
        	    D_flag += 1;
				if(!getDate(optarg,&criterion.pubDate)) {
                    fprintf(stderr, USAGE_MSG);
                    exit(1);
                }
                break;
            case 'G':
				G_flag += 1;
				criterion.genre = optarg;
                break;
            case 'N':
				N_flag += 1;
				criterion.name = optarg;
                break;
            case 'K':
				K_flag += 1;
				criterion.keyword = optarg;
                break;
            case 'H':
				fprintf(stdout, USAGE_MSG);
				return EXIT_SUCCESS;
            case 'n':
                if (!allDigits(optarg)) {
                    fprintf(stderr, USAGE_MSG);
                    exit(1);
                }
				NUM_arg = atoi(optarg);
                break;
            case 'o':
				OUTFILE = optarg;
                break;
            case 'a':
            case 'd':
                if (ORDER_arg != 'n')
                {
                    // already specified
                    fprintf(stderr, USAGE_MSG);
                    exit(1);
                }
				ORDER_arg = c;
                break;
            default:
                fprintf(stderr, USAGE_MSG);
                return EXIT_FAILURE;
        }
    }

    // validate a Search mode was specified - Does not check for more than 1
    if ( ! (I_flag | D_flag | G_flag | N_flag | K_flag) )
    {
        fprintf(stderr, "ERROR: Search mode was not specified.\n\n" USAGE_MSG);
        return EXIT_FAILURE;
    }
    
    // INSERT YOUR IMPLEMENTATION HERE
    // getopts only stored the arguments and performed basic checks. More error checking is still needed!!!!
   
    // EC: check same search criteria not specified more than once
    if ((I_flag > 1) || (D_flag > 1) ||(G_flag > 1) ||(N_flag > 1) ||(K_flag > 1))
    {
        fprintf(stderr, USAGE_MSG);
        exit(1);
    }

    void* comparer = NULL;
    switch (ORDER_arg)
    {
    case 'a':
        comparer = &book_tISBNAscComparator;
        break;
    case 'd':
    default:
        comparer = &book_tISBNDescComparator;
        break;
    }

    list_t* library = CreateList(comparer, &book_tPrinter, &book_tDeleter);
    char* buf = NULL;
    size_t bufSz = 0;
    // read csv
    while (getline(&buf, &bufSz, stdin) > 0)
    {
        // printf("READING %s\n", buf);
        book_t* nextBook = createBook(buf);
        if (nextBook == NULL)
        {
            // error in csv file
            exit(2);
        }
        // match book while reading to save memory
        if (bookMatch(nextBook, &criterion))
        {
            
            if (ORDER_arg == 'n')
            {
                InsertAtTail(library, nextBook);
            }
            else
            {
                InsertInOrder(library, nextBook);
            }
        }
        else
        {
            book_tDeleter(nextBook);
        }
    }
    // save memory
    free(buf);
    fclose(stdin);
    FILE* out;
    if (OUTFILE != NULL)
    {
        out = fopen(OUTFILE, "w");
        if (out == NULL)
        {
            // failed to create outfile
            exit(3);
        }
    }
    else
    {
        out = stdout;
    }

    if (I_flag && library->length > 1)
    {
        // cannot match more than 1 ISBN
        fclose(out);
        DestroyList(&library);
        exit(2);
    }

    // iterate through the linked list and search book
    node_t* ptr = library->head;
    int i = 0;
    while (ptr != NULL && (NUM_arg == 0 || i < NUM_arg))
    {
        library->printer(ptr->data, out, 1);
        ptr = ptr->next;
        ++i;
    }
    // clean up
    fclose(out);
    DestroyList(&library);
    errno = 0;
    exit(EXIT_SUCCESS);
}
