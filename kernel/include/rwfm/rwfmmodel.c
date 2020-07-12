//#include<stdlib.h>
//#include<stdio.h>

typedef unsigned long int USER_SET;
typedef unsigned int uint;

//Todo: Add descriptions for each struct field.
typedef struct subject {
    char compName[20];
    uint sub_id_index;
    uint owner;
    USER_SET readers;
    USER_SET writers;
} SUBJECT;

typedef struct object {
    char objName[20];
    uint obj_id_index;
    uint owner;					//index of the user id in ALL_UID array
    USER_SET readers;				//the bits in places of readers are 1
    USER_SET writers;				//the bits in places of writers are 1 
} OBJECT;

typedef struct epmapping { 			//struct for endpoint to camkes component ID and interface ID mapping
    uint epNo;
    uint compNo;
    char compName[20];
    uint intNo;
    char intName[20];
} epMapping;

typedef struct threadmapping {			//struct for thread id mapping with camkes component ID mapping
    uint thrNo;
    char thrName[20];
    uint compNo;
    char compName[20];
} thrMapping;



int add_user_to_label(int user_to_add, USER_SET * label);
int is_user_in_set(int user_id_index, USER_SET * set);
USER_SET set_union(USER_SET * set1, USER_SET * set2);
USER_SET set_intersection(USER_SET * set1, USER_SET * set2);
int is_subset_of(USER_SET * set1, USER_SET * set2);
int is_superset_of(USER_SET * set1, USER_SET * set2);
int do_read(uint sub_id_index,USER_SET *subject_readers,USER_SET *subject_writers,USER_SET *object_readers,USER_SET *object_writers);
int do_write(uint sub_id_index,USER_SET *subject_readers,USER_SET *subject_writers,USER_SET *object_readers,USER_SET *object_writers);
SUBJECT* get_sub_by_id(SUBJECT *sub_list,uint sub_id,int ns);
OBJECT* get_obj_by_id(OBJECT *obj_list,uint obj_id,int no);



int add_user_to_label(int user_to_add, USER_SET * label) {
    unsigned long long int max = -1;
    if(*label == max)
        return -1;

    USER_SET tmp = 1;
    tmp <<= user_to_add;

    *label |= tmp;

    return 0;
}

// int remove_user_from_set(int user_to_remove, USER_SET * set) {
//     unsigned long long int max = -1;
//     if(*set == max)
//         return -1;

//     USER_SET tmp = 1;
//     tmp <<= user_to_remove;
//     tmp = ~tmp;

//     *set &= tmp;

//     return 0;
// }

int is_user_in_set(int user_id_index, USER_SET * set) {
    USER_SET tmp = 1;
    tmp <<= user_id_index;
	
    return ((*set) & tmp) ? 1 : 0;
}

USER_SET set_union(USER_SET * set1, USER_SET * set2) {
    return *set1 | *set2;
}

USER_SET set_intersection(USER_SET * set1, USER_SET * set2) {
    return *set1 & *set2;
}

//Is set1 subset of set2
int is_subset_of(USER_SET * set1, USER_SET * set2) {
    USER_SET result = *set1 & *set2;
    if(*set1 == result)
        return 1;

    return 0;
}

//Is set1 superset of set2
int is_superset_of(USER_SET * set1, USER_SET * set2) {
    USER_SET result = *set1 | *set2;
    if(*set1 == result)
        return 1;

    return 0;
}

int do_read(uint sub_id_index,USER_SET *subject_readers,USER_SET *subject_writers,USER_SET *object_readers,USER_SET *object_writers)
{
    printf("\nSubject\nReaders:%ld\nWriters:%ld",*subject_readers,*subject_writers);
    printf("\nObject\nReaders:%ld\nWriters:%ld",*object_readers,*object_writers);
    if(is_user_in_set(sub_id_index,object_readers))         // Read Rule
    {
        *subject_readers = set_intersection(subject_readers,object_readers);  // Modify Reader Set
        *subject_writers = set_union(subject_writers,object_writers);        // Modify Writer Set
        printf("\nUpdated Labels");
        printf("\nSubject \nReaders:%ld\nWriters:%ld",*subject_readers,*subject_writers);
        printf("\nObject \nReaders:%ld\nWriters:%ld",*object_readers,*object_writers);
        return 0;
    }
    else
    {
        printf("\n***** ERROR : User is not in reader set *****\n");
        return 1;
    }
}

int do_write(uint sub_id_index,USER_SET *subject_readers,USER_SET *subject_writers,USER_SET *object_readers,USER_SET *object_writers)
{
    printf("\nSubject\nReaders:%ld\nWriters:%ld",*subject_readers,*subject_writers);
    printf("\nObject\nReaders:%ld\nWriters:%ld",*object_readers,*object_writers);
    if(is_user_in_set(sub_id_index,object_writers) && is_subset_of(object_readers,subject_readers) && is_superset_of(object_writers,subject_writers))
    return 0;         // Write Rule
    else
    {
        printf("\n***** ERROR : Write rule is violated *****\n");
        return 1;
    }
}

SUBJECT* get_sub_by_id(SUBJECT *sub_list,uint sub_id,int ns)
{
    for(int i=0;i<ns;i++)
    {
        if(sub_list[i].sub_id_index == sub_id)
        return &sub_list[i];
    }
    return NULL;
}

OBJECT* get_obj_by_id(OBJECT *obj_list,uint obj_id,int no)
{
    for(int i=0;i<no;i++)
    {
        if(obj_list[i].obj_id_index == obj_id)
        return &obj_list[i];
    }
    return NULL;
}


/*int main()
{
    USER_SET ALL_UID = 0;									// all ids
    USER_SET temp;
    int ns,no,nr,nw,ri,wi;
    
    // Subject
    
    printf("Enter number of subjects to add : ");
    scanf("%d",&ns);
    SUBJECT *s = (SUBJECT *)malloc(ns*sizeof(SUBJECT));
    for(int i=0;i<ns;i++)
    {
        printf("\nEnter subject id : ");
        scanf("%d",&s[i].sub_id_index);
        printf("\nEnter owner id : ");
        scanf("%d",&s[i].owner);
        temp = 1;
        ALL_UID|=temp<<s[i].sub_id_index;
        temp = 1;
        s[i].writers = temp<<s[i].sub_id_index;
    }
    
    for(int i=0;i<ns;i++)
    s[i].readers = ALL_UID;
    
    // Object

    printf("\nEnter number of objects to add : ");
    scanf("%d",&no);
    OBJECT *o = (OBJECT *)malloc(no*sizeof(OBJECT));
    for(int i=0;i<no;i++)
    {
        printf("\nEnter object id : ");
        scanf("%d",&o[i].obj_id_index);
        printf("\nEnter owner id : ");
        scanf("%d",&o[i].owner);
        o[i].readers = 0;
        printf("\nEnter number of Readers: ");
        scanf("%d",&nr);
        while(nr--)
        {
            printf("\nEnter id of Reader : ");
            scanf("%d",&ri);
            add_user_to_label(ri,&o[i].readers);
        }
        o[i].writers = 0;
        printf("\nEnter number of Writers: ");
        scanf("%d",&nw);
        while(nw--)
        {
            printf("\nEnter id of writers : ");
            scanf("%d",&wi);
            add_user_to_label(wi,&o[i].writers);
        }


    }
    
    int ch;
    uint sub_id,obj_id;
    while(1)
    {
        printf("\nPress 1 to Read \n2 to Write \nAny other number to exit : \n");
        scanf("%d",&ch);
        switch(ch)
        {
            case 1: printf("\nEnter subject id who is reading : ");
                    scanf("%d",&sub_id);
                    printf("\nEnter object id for reading : ");
                    scanf("%d",&obj_id);
                    SUBJECT *sub = get_sub_by_id(s,sub_id,ns);
                    OBJECT *obj = get_obj_by_id(o,obj_id,no);
                    if (sub == NULL || obj == NULL)
                    	printf("\nEither subject or object does not exist");
                    else
                    {
                    	if(!do_read(sub_id,&sub->readers,&sub->writers,&obj->readers,&obj->writers))
                    		printf("\nRead Successfully\n");
                    }
                    break;

            case 2: printf("\nEnter subject id who is writing : ");
                    scanf("%d",&sub_id);
                    printf("\nEnter object id for writing : ");
                    scanf("%d",&obj_id);
                    sub = get_sub_by_id(s,sub_id,ns);
                    obj = get_obj_by_id(o,obj_id,no);
                    if (sub == NULL || obj == NULL)
                    	printf("\nEither subject or object does not exist");
                    else
                    {
                    	if(!do_write(sub_id,&sub->readers,&sub->writers,&obj->readers,&obj->writers))
                    		printf("\nWrote Successfully\n");
                    }
                    break;            
            default : return 1;
        }
        
    }
    return 0;
}
*/
/*void read_labels()
{
    USER_SET ALL_UID = 0;									// all ids
    USER_SET temp;
    int ns,no,nr,nw,ri,wi;
    
    FILE *fptr;
    fptr = fopen("same_page/rwfm_labels", "r");
    if(fptr == NULL)
    {
    	printf("Cannot open file \n"); 
    	exit(0); 
    }

    printf("%d %d", getw(fptr), getw(fptr));
    // Subject
 */   
/*    printf("Enter number of subjects to add : ");
    scanf("%d",&ns);
    SUBJECT *s = (SUBJECT *)malloc(ns*sizeof(SUBJECT));
    for(int i=0;i<ns;i++)
    {
        printf("\nEnter subject id : ");
        scanf("%d",&s[i].sub_id_index);
        printf("\nEnter owner id : ");
        scanf("%d",&s[i].owner);
        temp = 1;
        ALL_UID|=temp<<s[i].sub_id_index;
        temp = 1;
        s[i].writers = temp<<s[i].sub_id_index;
    }
    
    for(int i=0;i<ns;i++)
    s[i].readers = ALL_UID;
    
    // Object

    printf("\nEnter number of objects to add : ");
    scanf("%d",&no);
    OBJECT *o = (OBJECT *)malloc(no*sizeof(OBJECT));
    for(int i=0;i<no;i++)
    {
        printf("\nEnter object id : ");
        scanf("%d",&o[i].obj_id_index);
        printf("\nEnter owner id : ");
        scanf("%d",&o[i].owner);
        o[i].readers = 0;
        printf("\nEnter number of Readers: ");
        scanf("%d",&nr);
        while(nr--)
        {
            printf("\nEnter id of Reader : ");
            scanf("%d",&ri);
            add_user_to_label(ri,&o[i].readers);
        }
        o[i].writers = 0;
        printf("\nEnter number of Writers: ");
        scanf("%d",&nw);
        while(nw--)
        {
            printf("\nEnter id of writers : ");
            scanf("%d",&wi);
            add_user_to_label(wi,&o[i].writers);
        }


    }
    
    int ch;
    uint sub_id,obj_id;
    while(1)
    {
        printf("\nPress 1 to Read \n2 to Write \nAny other number to exit : \n");
        scanf("%d",&ch);
        switch(ch)
        {
            case 1: printf("\nEnter subject id who is reading : ");
                    scanf("%d",&sub_id);
                    printf("\nEnter object id for reading : ");
                    scanf("%d",&obj_id);
                    SUBJECT *sub = get_sub_by_id(s,sub_id,ns);
                    OBJECT *obj = get_obj_by_id(o,obj_id,no);
                    if (sub == NULL || obj == NULL)
                    	printf("\nEither subject or object does not exist");
                    else
                    {
                    	if(!do_read(sub_id,&sub->readers,&sub->writers,&obj->readers,&obj->writers))
                    		printf("\nRead Successfully\n");
                    }
                    break;

            case 2: printf("\nEnter subject id who is writing : ");
                    scanf("%d",&sub_id);
                    printf("\nEnter object id for writing : ");
                    scanf("%d",&obj_id);
                    sub = get_sub_by_id(s,sub_id,ns);
                    obj = get_obj_by_id(o,obj_id,no);
                    if (sub == NULL || obj == NULL)
                    	printf("\nEither subject or object does not exist");
                    else
                    {
                    	if(!do_write(sub_id,&sub->readers,&sub->writers,&obj->readers,&obj->writers))
                    		printf("\nWrote Successfully\n");
                    }
                    break;            
            default : return 1;
        }
        
    }
    return 0;
    */
//}
