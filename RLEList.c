#include "RLEList.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define TURN_TO_STRING(a) (#a)

typedef struct RLEList_t{
    char letter;
    int occur;
    RLEList next;
} RLEList_t;

RLEList RLEListCreate()
{
    RLEList node = malloc(sizeof(RLEList_t));
    if (node == NULL)
    {
        return NULL;
    }
    node->letter = 0;
    node->occur = 0;
    node->next = NULL;
    return node;
}

void RLEListDestroy(RLEList list)
{
    while (list) //list != NULL
    {
        RLEList toDelete = list;
        list = list->next;
        free(toDelete);
    }
    return;
}

RLEListResult RLEListAppend(RLEList list, char value)
{
    if ((list == NULL) || (value == 0))
    {
        return RLE_LIST_NULL_ARGUMENT;
    }
    // we check to see if the the element we're adding is the same as the last one
    while (list->next) //list->next != NULL
    {
        list = list->next;
    }
    // if its the same we increment the occur by one
    if ((list->letter == value) || ((list->letter == 0) && (list->occur == 0)))
    {
        list->letter = value;
        list->occur += 1;
    }
        // if not we create a new node (after the dummy node if the list is "empty (empty here means it only has the dummy node")
    else
    {
        RLEList newNode = malloc(sizeof(RLEList_t));
        if (newNode == NULL)
        {
            return RLE_LIST_OUT_OF_MEMORY;
        }
        // we are sure to have a new node now
        list->next = newNode;
        newNode->letter = value;
        newNode->occur = 1;
        newNode->next = NULL;
    }
    return RLE_LIST_SUCCESS;
}

int RLEListSize(RLEList list)
{
    if (list == NULL)
    {
        return -1;
    }
    int counter = 0;
    while (list) // list != NULL
    {
        counter += list->occur;
        list = list->next;
    }
    return counter;
}

RLEListResult RLEListRemove(RLEList list, int index)
{
    if(list == NULL)
    {
        return RLE_LIST_NULL_ARGUMENT;
    }
    int listLength = RLEListSize(list);
    if ((index < 0) || (index >= listLength))
    {
        return RLE_LIST_INDEX_OUT_OF_BOUNDS;
    }
    // after these checks we are guarnteed to have a non-empty list
    /* prevnode will point to the previous node in the given list
     in case that <list> is pointing the first node, the prevNode will point to the first node as well
     otherwise it will point to the previous node that the list points at
     */
    RLEList currNode = list;
    RLEList prevNode = list;
    int counter = 0;
    // after the while loop, currNode will be pointing to the node we want to remove a letter from
    while (currNode->next) { // do this while we arent at the last node
        counter += currNode->occur;
        if (index < counter)
        {
            break;
        }
        if (currNode != prevNode)
        {
            prevNode = prevNode->next;
        }
        currNode = currNode->next;
    }
    assert(counter < listLength);
    assert(listLength > 0);
    assert(currNode != NULL);
    assert(currNode->occur != 0);
    assert(currNode->letter != 0);
    // after the while loop we have currNode pointing at node we want to remove from
    // there are 2 basic casses
    // case 1 when we remove on letter when there is more than 1
    if (currNode->occur > 1)
    {
        currNode->occur -= 1;
    }
    else // currNode->occur == 1
    {
        assert(currNode->occur == 1);
        if (list->next == NULL)
        {
            list->occur = 0;
            list->letter = 0;
        }
        else // list->next != NULL , this means we at least have 2 nodes
        {
            if (prevNode == currNode) // should "delete" the "first" node
            {
                // move the contents of the second node to the first one and then delete the second node
                RLEList toDelete = list->next;
                list->letter = list->next->letter;
                list->occur = list->next->occur;
                list->next = list->next->next;
                free(toDelete);
            }
            else
            {
                assert(prevNode->next == currNode);
                RLEList toDelete = currNode;
                currNode = currNode->next;
                prevNode->next = currNode;
                free(toDelete);
                if ((currNode != NULL) && (prevNode->letter == currNode->letter))
                {
                    prevNode->occur += currNode->occur;
                    RLEList toDelete = currNode;
                    currNode = currNode->next;
                    prevNode->next = currNode;
                    free(toDelete);
                }
            }
        }
    }
    //printf("\nits 7\n");
    // we have everything removed now
    return RLE_LIST_SUCCESS;
}


char RLEListGet(RLEList list, int index, RLEListResult *result)
{
    if (list == NULL)
    {
        if (result)
        {
            *result = RLE_LIST_NULL_ARGUMENT;
        }
        return 0;
    }
    // check if in bounds
    int listLength = RLEListSize(list);
    if ((index < 0) || (index >= listLength))
    {
        if (result)
        {
            *result = RLE_LIST_INDEX_OUT_OF_BOUNDS;
        }
        return 0;
    }
    int counter = 0;
    while (list->next)
    {
        counter += list->occur;
        if (index < counter)
        {
            break;
        }
        list = list->next;
    }
    if (result)
    {
        *result = RLE_LIST_SUCCESS;
    }
    return list->letter;
}

char* RLEListExportToString(RLEList list, RLEListResult* result)
{
    if (list == NULL)
    {
        if (result)
        {
            *result = RLE_LIST_NULL_ARGUMENT;
        }
        return 0;
    }
    RLEList temp = list;
    int stringLength = 1;
    while (temp && !((temp->letter == 0) && (temp->occur == 0)))
    {
        stringLength += 1 + strlen(TURN_TO_STRING(temp->occur)) + 1;
        temp = temp->next;
    }
    char *string = malloc(stringLength);
    if (string == NULL)
    {
        // what should we return?
        if (result)
        {
            *result = RLE_LIST_OUT_OF_MEMORY;
        }
        return NULL;
    }
    int i = 0;
    while (list->next)
    {
        list = list->next;
        string[i] = list->letter;
        strcpy(string+i+1, TURN_TO_STRING(list->occur));
        i += 1 + strlen(TURN_TO_STRING(list->occur));//???????????????????????????
        string[i] = '\n';
        i++;
    }
    string[i] = '\0';
    if (result)
    {
        *result = RLE_LIST_SUCCESS;
    }
    return string;
}

RLEListResult RLEListMap(RLEList list, MapFunction map_function)
{
    if (list == NULL)
    {
        return RLE_LIST_NULL_ARGUMENT;
    }
    RLEList temp = list;
    while (temp)
    {
        temp->letter = map_function(temp->letter);
        temp = temp->next;
    }
    RLEList nextNode = list->next;
    while (nextNode)
    {
        if(list->letter == nextNode->letter)
        {
            list->occur += nextNode->occur;
            RLEList toDelete = nextNode;
            nextNode = nextNode->next;
            list->next = nextNode;
            free(toDelete);
        }
        list = list->next;
        if(nextNode)
        {
            nextNode = nextNode->next;
        }
    }
    return RLE_LIST_SUCCESS;
}
