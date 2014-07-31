 #include<stdio.h>
    #include<stdlib.h>

    struct stack
    {
    int info;
    struct stack *link;
    };
    struct stack *start=NULL;

    int main()
    {
    int ch;
    while(ch!=6)
    {
    printf("1 for Push\n");
    printf("2 for Pop\n");
    printf("3 for Peep\n");
    printf("4 for Change\n");
    printf("5 for Display\n");
    printf("6 for exit\n");
    printf("Enter ur choice:");
    scanf("%d",&ch);
    switch(ch)
    {
    case 1: push();
        break;
    case 5: display();
        break;
    case 6: break;
    }
    }
    }

    int push()
    {
     struct stack *new,*temp;
     int i=0;
     for(temp=start;temp!=NULL;temp=temp->link)
     {
     i++;
     }
     if(i==5)
     {
     printf("Stack Overflow");
     return 0;
     }
     else
     {
     new=(struct stack *)malloc(sizeof(struct stack));
     printf("Enter value for stack: ");
     scanf("%d",&new->info);
     new->link=start;
     start=new;
     }
     }


    int display()
    {
    struct stack *temp;
    printf("\n****Stack Values****\n");
    for(temp=start;temp!=NULL;temp=temp->link)
       {
       printf("%d\n",temp->info);
       }
    return 0;
    }

