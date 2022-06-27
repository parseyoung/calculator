#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// �߻��� �� �ִ� ���� ���
#define COMPLETE 0                  // �������� ��
#define OPERATOR_NOT_DEFINED 1      // ���ǵ��� ���� �����ڰ� ���Ŀ� ��Ÿ�� ���
#define OPERAND_NOT_FOUND 2         // �ǿ����ڸ� ã�� ���� ��� (�� : 3 + * 5 ó�� ���� �ǿ����ڰ� ���� �ڸ��� �����ڰ� �������� ��)
#define DIVIDE_BY_ZERO 3            // 0���� �������� �õ��� ���
#define PARENTHESE_PAIR_ERROR 4     // ��ȣ�� ¦�� ���� �ʴ� ���
#define UNDEFINE_ERROR 99           // �� �� �� �� ���� ����

typedef struct StringNode
{
    char* Value;
    struct StringNode* Next;
} StringNode;
// ť �Ǵ� ����ó�� �� �� �ִ� LinkedList
typedef struct StackQueue
{
    StringNode* Head;
} StackQueue;

int IsEmptyStackQueue(StackQueue* sq)
{
    return (sq->Head == NULL);
}
// ����Ʈ�� �� �ڿ� ��带 �ִ´�.
void PushBack(StackQueue* sq, StringNode* node)
{
    if (sq->Head == NULL)
    {
        sq->Head = node;
        return;
    }

    StringNode* current = sq->Head;
    while (current->Next != NULL)
    {
        current = current->Next;
    }

    current->Next = node;
}
StringNode* DequeueStackQueue(StackQueue* sq)
{
    if (sq->Head == NULL) return NULL;
    // ������ ����Ʈ������ ������ �ڿ� �����Ƿ�, ť��� �����ϸ� �� �տ��� ������ �ȴ�.

    StringNode* result = sq->Head;
    sq->Head = result->Next;
    return result;
}
StringNode* PopStackQueue(StackQueue* sq)
{
    if (sq->Head == NULL) return NULL;

    // ������ ����Ʈ������ ������ �ڿ� �����Ƿ�, �����̶�� �����ϸ� �� �ڿ��� ������ �ȴ�.
    StringNode* current = sq->Head;

    if (current->Next == NULL)
    {
        sq->Head = NULL;
        return current;
    }

    while (current != NULL)
    {
        if (current->Next != NULL && current->Next->Next == NULL)
        {
            StringNode* result = current->Next;
            current->Next = NULL;
            return result;
        }
        else current = current->Next;
    }
}
StringNode* CreateStringNode(char* value)
{
    StringNode* newNode = (StringNode*)malloc(sizeof(StringNode));
    newNode->Value = value;
    newNode->Next = NULL;
}
void InitStackQueue(StackQueue* sq)
{
    sq->Head = NULL;
}
void DeleteStringNode(StringNode* node)
{
    if (node == NULL) return;

    if (node->Value != NULL) free(node->Value);
    free(node);
}

// �������� ������ ������ ������
typedef enum Type
{
    Open = 0,
    Close = 0,
    Plus = 1,
    Minus = 1,
    Multiply = 2,
    Divide = 2,
    Number,
    Unknown
} Type;

typedef struct NodeType NodeType;
// �����͸� ������ ��� ����ü
struct NodeType
{
    NodeType* Next;
    Type Type;      // ���� ��忡 ����� �����Ͱ� ���������� ���������� ��Ÿ����, �������� ��� �������� ������ �����Ѵ�. (�� Type ������ ����)
    double Value;   // ���� Type�� Number�� ��� ���ڰ��� �̰��� ������
    char Operator;  // ���� Type�� Number�� �ƴ� ��� �����ڸ� �̰��� ������
};
// ���ο� ��带 �����ϴ� �Լ�
NodeType* CreateNode(Type type, double value, char op)
{
    NodeType* node = (NodeType*)malloc(sizeof(NodeType));

    node->Type = type;
    if (type == Number)
    {
        node->Value = value;
    }
    else
    {
        node->Operator = op;
    }

    return node;
}

typedef struct StackType
{
    NodeType* Top;
} StackType;

void Init(StackType* s)
{
    s->Top = NULL;
}
void Push(StackType* s, NodeType* n)
{
    n->Next = s->Top;
    s->Top = n;
}
// Pop�� ������ Top���� �����͸� ������, �ش� �����͸� ��ȯ�Ͽ��� �Ѵ�.
// ������ ���� �����ʹ� �����Ҵ�� �������̱� ������ free�� ��ȯ ���� ����ڰ� ���� ó���Ͽ��� �ϴµ�,
// �̸� �����ϱ� ���� ���� �������� ���纻�� �����ϰ�, ���� �����ʹ� ������ �� ���纻�� �ּҸ� ��ȯ�Ѵ�.
NodeType* Pop(StackType* s)
{
    if (IsEmpty(s)) return NULL;

    NodeType* n = s->Top;
    s->Top = n->Next;

    static NodeType result;
    result.Type = n->Type;
    result.Value = n->Value;
    result.Operator = n->Operator;
    result.Next = n->Next;

    free(n);

    return &result;
}
int IsEmpty(StackType* s)
{
    return s->Top == NULL;
}
NodeType* Peek(StackType* s)
{
    return s->Top;
}

char* RpnConvert(char* infixExpr, int* errorCode);
int Evaluate(char* postfixExpr, double* result);

// �־��� ���� �ڵ忡 �ش��ϴ� ���� �޽����� ������ִ� �Լ�
void printError(int errorCode)
{
    char* errorMessage;

    switch (errorCode)
    {
    case COMPLETE:
        errorMessage = "";
        break;

    case OPERATOR_NOT_DEFINED:
        errorMessage = "ERROR : Operator not defined.";
        break;

    case OPERAND_NOT_FOUND:
        errorMessage = "ERROR : Operand not found.";
        break;

    case DIVIDE_BY_ZERO:
        errorMessage = "ERROR : Divide by zero.";
        break;

    case PARENTHESE_PAIR_ERROR:
        errorMessage = "ERROR : Parenthese pair not matched.";
        break;

    case UNDEFINE_ERROR:
    default:
        errorMessage = "ERROR : Undefined error.";
        break;
    }

    printf("%s\n", errorMessage);
}

int main(int argc, char* argv[])
{
    // ���� �Ű������� ������ 2�� ������ ��� ������ �˷��ְ� ���α׷� ����
    if (argc < 3)
    {
        printf("./Calc Equation.txt Flag(0:Queue 1:Stack)\n");
        exit(1);
    }

    int flag;
    if (*argv[2] == '0') flag = -1;
    else if (*argv[2] == '1') flag = 1;
    else flag = 0;

    // ������ �б� ���� ����.
    FILE* fp = fopen(argv[1], "r");

    // ���� ���� ������ ��� ���α׷� ����
    if (fp == NULL)
    {
        printf("File not found.\n");
        exit(1);
    }

    StackQueue sq;
    StackQueue tmp;
    InitStackQueue(&sq);
    InitStackQueue(&tmp);

    char buffer[256];
    // ���Ϸκ��� ������ ���� ������ ���ڿ��� �� ������ �д´�.
    printf("Input Equation\n");
    while (!feof(fp))
    {
        buffer[0] = '\0';

        fgets(buffer, 256, fp);

        if (strlen(buffer) == 0) continue;

        buffer[strlen(buffer) - 1] = '\0';

        printf("Equation = %s\n\n", buffer);

        char* value = (char*)malloc(sizeof(char) * strlen(buffer) + 1);
        strcpy(value, buffer);

        if (flag) PushBack(&sq, CreateStringNode(value));
    }

    fclose(fp);

    if (!flag)
    {
        printf("Type the Correct Flag\n");
        exit(1);
    }

    if (flag == 1)
    {
        printf("Stack Equation\n");
    }
    else
    {
        printf("Queueing Equation\n");
    }

    while (!IsEmptyStackQueue(&sq))
    {
        StringNode* node;
        // Queue
        if (flag == -1)
        {
            node = DequeueStackQueue(&sq);
        }
        // Stack
        else
        {
            node = PopStackQueue(&sq);
        }

        /*strcpy(buffer, node->Value);
        DeleteStringNode(node);*/

        printf("Equation = %s\n\n", node->Value);
        // ������ ���ŷ����� ���ÿ� �ִ°� ���� �����ְ� Result�� �� ���� ����ؾ� �ϹǷ� �ĵ��� ������ �ְ� �ٸ� ����Ʈ�� �Űܾ� �Ѵ�
        // ��, node�� Next�� NULL�� �ٲ۴�.
        node->Next = NULL;
        PushBack(&tmp, node);
    }

    printf("\n\n");
    while(!IsEmptyStackQueue(&tmp))
    {
        StringNode* node;

        // ���� �����̾��ٸ� ���� sq�� �ִ��� tmp�� �ű�� �������� ����Ʈ�� �������� �ٲ�����Ƿ� Pop ��� Dequeue�� �ݴ�� �����;� �����̵ȴ�.
        // Queue
        if (flag == -1)
        {
            node = DequeueStackQueue(&tmp);
        }
        // Stack
        else
        {
            node = DequeueStackQueue(&tmp);
        }

        strcpy(buffer, node->Value);
        DeleteStringNode(node);

        int errorCode;
        // ���� ����ǥ����� ����ǥ������� ��ȯ�Ѵ�.
        char* result = RpnConvert(buffer, &errorCode);
        if (errorCode == COMPLETE)
        {
            // ��ȯ�� ����ǥ����� �ӽ� ���۷� �����´�.
            strcpy(buffer, result);
            free(result);

            double evalResult;
            // ����ǥ����� ����Ѵ�.
            errorCode = Evaluate(buffer, &evalResult);
            if (errorCode == COMPLETE)
            {
                // ����� ���
                printf("Result = %f\n", evalResult);
            }
            else
            {
                // ��� ���н� ���� �ڵ忡 ���� ���� �޽��� ���
                printError(errorCode);
            }
        }
        else
        {
            // ��ȯ ���н� ���� �ڵ忡 ���� ���� �޽��� ���
            printError(errorCode);
        }
    }

    return 0;
}

// ���ڿ� str�� position ��ġ���� �����ؼ� ��ū�� �ϳ� �о ���� ����� ��ȯ�ϴ� �Լ�
NodeType* ReadNextToken(char* str, int* position)
{
    // ���� ���ڿ��� ���� �о��ٸ� NULL ��ȯ
    if (str[*position] == '\0') return NULL;

    // �켱 ���������� �˻��ϱ� ���� ���ڸ� �ϳ��� �о�´�.
    char check = str[(*position)++];
    // �о�� ���ڿ� �ش��ϴ� �����ڰ� �ִ��� �˻��Ͽ�, �����ڰ� �´ٸ� �����ڸ� ���ο� ��忡 �־ ��ȯ�Ѵ�.
    switch (check)
    {
    case '+':
        return CreateNode(Plus, 0, '+');
    case '-':
        return CreateNode(Minus, 0, '-');
    case '*':
        return CreateNode(Multiply, 0, '*');
    case '/':
        return CreateNode(Divide, 0, '/');
    case '(':
        return CreateNode(Open, 0, '(');
    case ')':
        return CreateNode(Close, 0, ')');
    }

    // ������ ��ȯ���� �ʾҴٸ� �ϴ� �����ڴ� �ƴϹǷ� ���ڸ� �޾Ƶ��� �غ� �Ѵ�.
    char buffer[20];
    int index = 1;

    // ó���� �ϳ� �о��� ���ڴ� ������ �ȵǱ� ������ �ӽ� ������ ù �ڸ��� ��ġ��Ų��.
    buffer[0] = check;

    // str�� position���� ���� �Ǵ� �� ���ڸ� ���� ������ �ݺ��Ѵ�.
    for (; str[*position] != ' ' && str[*position] != '\0'; (*position)++)
    {
        // ���� �����ڱ��� �о���� ���� �ֱ� ������ ������ ���� �ʰ� �ϴ� �ϳ� �����ͼ� üũ�Ѵ�.
        char preview = str[*position];
        // ���� �о�� ���ڰ� �Ҽ����� �ǹ��ϴ� .�̰ų�, 0~9 ������ ���ڶ��
        if (preview == '.' || (preview >= '0' && preview <= '9'))
        {
            // ���ۿ� �־��ְ�,
            buffer[index++] = str[*position];
        }
        else
        {
            // �װ� �ƴ϶�� ���� �����ڸ� �о���� ���̹Ƿ� ���� ����
            break;
        }
    }

    // �ӽ� ������ �������� �� ���� �߰�
    buffer[index] = '\0';

    // �ӽ� ������ ���� �Ǽ� ������ ��ȯ�Ѵ�.
    double value = atof(buffer);

    // atof() �Լ��� ���ڿ��� �Ǽ��� ��ȯ�ϴ� �Լ��̴�.
    // �Ǽ��� ��ȯ�� ������ ��� ��ȯ�� �Ǽ�����, ������ ��� 0�� ��ȯ�Ѵ�.
    // ���� ��ȯ���� 0�� ��� ������ ���� ���ڿ��� 0�� ��Ÿ�´� ������ Ȯ���� �ʿ䰡 �ִ�.
    if (value == 0)
    {
        int dotCount = 0;
        int dotIndex = -1;
        for (int i = 0; buffer[i]; i++)
        {
            if (buffer[i] != '0')
            {
                if (buffer[i] == '.')
                {
                    dotCount++;
                    dotIndex = i;
                }
                // ���� ���ڿ��� ���� ���� �߿� '0'�̳� '.'�� �ƴ� �ٸ� ���ڰ� �ִٸ� ������ �Ǽ����� �ƴϴ�.
                else return CreateNode(Unknown, 0, '\0');
            }
        }
        // ���ڿ��� '0'�̳� '.'���θ� �����Ǿ��־�����, '.'�� �� �� �̻� �����߰ų�, '.'�� ��ġ�� ���ڿ��� �� �� �Ǵ� �� ���� ��� �Ǽ����� �ƴ϶�� �� �� �ִ�.
        if (dotCount != 1 || dotIndex == 0 || dotIndex == strlen(buffer) - 1) return CreateNode(Unknown, 0, '\0');
    }

    // ��ȯ �Ϸ�� ���ڸ� ���ο� ��忡 �־ ��ȯ�Ѵ�.
    return CreateNode(Number, value, '\0');
}

// ���ڿ��� ���� �������� ���� ���� �̾�ٿ��ִ� �Լ�
void AppendValue(char* dest, double value)
{
    char buffer[20];
    sprintf(buffer, "%f ", value);

    strcat(dest, buffer);
}
// ���ڿ��� ���� �������� ���� ���� �̾�ٿ��ִ� �Լ�
void AppendChar(char* dest, char c)
{
    char buffer[3] = { c, ' ', '\0' };

    strcat(dest, buffer);
}

// ����ǥ����� ����ǥ������� ��ȯ�ϴ� �Լ�
// ��ȯ�� : �Լ� ���� �� errorCode�� ������� COMPLETE�� ��� ��ȯ�� ����ǥ���, COMPLETE�� �ƴ� ��� NULL
// char* infixExpr : ��ȯ�� ����ǥ���
// int* errorCode : ���� �ڵ�
char* RpnConvert(char* infixExpr, int* errorCode)
{
    StackType s;
    int checkError;
    char* result = (char*)calloc(100, sizeof(char));
    int index = 0, position = 0;

    Init(&s);

    while (1)
    {
        if (infixExpr[position] == ' ') position++;
        NodeType* o1 = ReadNextToken(infixExpr, &position);
        if (o1 == NULL) break;

        if (o1->Type == Number)
        {
            AppendValue(result, o1->Value);
        }
        else
        {
            switch (o1->Operator)
            {
            case '+':
            case '-':
            case '*':
            case '/':
                // ��ū�� �������� ��� ���ÿ� �����ڰ� �������� ���� ������ �Ʒ��� �ݺ��Ѵ�.
                while (!IsEmpty(&s))
                {
                    NodeType* op2 = Peek(&s);
                    // o1�� �°��� ������(+, -, *, /)�̰�, o2���� �켱������ ���ų� ���� ���,
                    if (((o1->Type == 1 || o1->Type == 2) && op2->Type >= o1->Type) ||
                        // �켱������ o2���� ���� ���
                        (o1->Type < op2->Type))
                    {
                        op2 = Pop(&s);
                        AppendChar(result, op2->Operator);
                    }
                    else break;
                }
                Push(&s, o1);
                break;

            case '(':
                Push(&s, o1);
                break;

            case ')':
                // ���� ��ȣ�� ���� ������ �����ڸ� ���Ͽ� ����Ѵ�.
                checkError = 1;
                while (!IsEmpty(&s))
                {
                    NodeType* op2 = Pop(&s);
                    if (op2->Type == Open)
                    {
                        checkError = 0;
                        break;
                    }

                    AppendChar(result, op2->Operator);
                }

                // ���� ��ȣ�� ã�� ���� ��� - ��ȣ �� ����
                if (checkError)
                {
                    *errorCode = PARENTHESE_PAIR_ERROR;
                    return NULL;
                }
                break;

            default:
                *errorCode = OPERATOR_NOT_DEFINED;
                return NULL;
            }
        }
    }

    while (!IsEmpty(&s))
    {
        NodeType* op = Pop(&s);
        // ���ÿ� ���� ��Ұ� ��ȣ�� ��쿡�� ������ ��ȣ�� ��� ó������ ���� �� ��, ��ȣ�� ¦�� �ȸ´� ���̹Ƿ� ����.
        // Type �������� Open(���� ��ȣ)�� Close(�ݴ� ��ȣ)�� �� �� 0���� ����Ǿ� �����Ƿ� Open�� ���ص� �ȴ�.
        if (op->Type == Open)
        {
            *errorCode = PARENTHESE_PAIR_ERROR;
            return NULL;
        }
        else AppendChar(result, op->Operator);
    }

    *errorCode = COMPLETE;
    return result;
}
// ����ǥ��� ����Լ�
// ��ȯ�� : ���� �ڵ�
// char* postfixExpr : ����� ����ǥ���
// double* result : ��ȯ�� ���� �ڵ尡 COMPLETE�� ��� ��� ���, COMPLETE�� �ƴ� ��� �� �� ���� ��
int Evaluate(char* postfixExpr, double* result)
{
    StackType s;
    int position = 0;

    Init(&s);

    while (1)
    {
        if (postfixExpr[position] == ' ') position++;
        NodeType* token = ReadNextToken(postfixExpr, &position);

        if (token == NULL) break;

        // Token�� Number�� ��쿡�� ���ÿ� Ǫ���Ѵ�.
        if (token->Type == Number)
        {
            Push(&s, token);
        }
        else
        {
            // Token�� Operator�� ��쿡�� ���ÿ��� �����ڿ� �˸��� ������ Number�� �����ͼ� �����Ѵ�.
            double op1, op2;

            NodeType* o2 = Pop(&s);
            if (o2 == NULL) return OPERAND_NOT_FOUND;
            op2 = o2->Value;

            NodeType* o1 = Pop(&s);
            if (o1 == NULL) return OPERAND_NOT_FOUND;
            op1 = o1->Value;

            // �� �����ڿ� �°� ������ ������� ���ÿ� Ǫ��
            switch (token->Operator)
            {
            case '+':
                Push(&s, CreateNode(Number, op1 + op2, '\0'));
                break;

            case '-':
                Push(&s, CreateNode(Number, op1 - op2, '\0'));
                break;

            case '*':
                Push(&s, CreateNode(Number, op1 * op2, '\0'));
                break;

            case '/':
                if (op2 == 0) return DIVIDE_BY_ZERO;
                Push(&s, CreateNode(Number, op1 / op2, '\0'));
                break;

            default:
                return OPERATOR_NOT_DEFINED;
            }
        }
    }

    // ������ ����ų� ���ÿ� 2�� �̻��� ��Ұ� �����ִ� ��� ����.
    if (IsEmpty(&s) || Peek(&s)->Next != NULL) return OPERAND_NOT_FOUND;
    else if (Peek(&s)->Type != Number) return UNDEFINE_ERROR;

    *result = Pop(&s)->Value;
    return COMPLETE;
}