struct elem
{
 int id;
 int clock;
 struct elem *next;
}; 

int insertElem(struct elem legion,struct elem* head);
int removeElem(int id,struct elem* head);
int sum_before(int id, int legions[],struct elem* head);