#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 발생할 수 있는 오류 목록
#define COMPLETE 0                  // 성공했을 때
#define OPERATOR_NOT_DEFINED 1      // 정의되지 않은 연산자가 수식에 나타난 경우
#define OPERAND_NOT_FOUND 2         // 피연산자를 찾지 못한 경우 (예 : 3 + * 5 처럼 원래 피연산자가 나온 자리에 연산자가 등장했을 때)
#define DIVIDE_BY_ZERO 3            // 0으로 나눗셈을 시도한 경우
#define PARENTHESE_PAIR_ERROR 4     // 괄호의 짝이 맞지 않는 경우
#define UNDEFINE_ERROR 99           // 그 외 알 수 없는 오류

typedef struct StringNode
{
    char* Value;
    struct StringNode* Next;
} StringNode;
// 큐 또는 스택처럼 쓸 수 있는 LinkedList
typedef struct StackQueue
{
    StringNode* Head;
} StackQueue;

int IsEmptyStackQueue(StackQueue* sq)
{
    return (sq->Head == NULL);
}
// 리스트의 맨 뒤에 노드를 넣는다.
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
    // 어차피 리스트에서는 무조건 뒤에 넣으므로, 큐라고 가정하면 맨 앞에서 꺼내면 된다.

    StringNode* result = sq->Head;
    sq->Head = result->Next;
    return result;
}
StringNode* PopStackQueue(StackQueue* sq)
{
    if (sq->Head == NULL) return NULL;

    // 어차피 리스트에서는 무조건 뒤에 넣으므로, 스택이라고 가정하면 맨 뒤에서 꺼내면 된다.
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

// 연산자의 종류를 정의한 열거형
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
// 데이터를 저장할 노드 구조체
struct NodeType
{
    NodeType* Next;
    Type Type;      // 현재 노드에 저장된 데이터가 연산자인지 숫자인지를 나타내며, 연산자인 경우 연산자의 종류를 저장한다. (위 Type 열거형 참고)
    double Value;   // 위의 Type이 Number인 경우 숫자값을 이곳에 저장함
    char Operator;  // 위의 Type이 Number가 아닌 경우 연산자를 이곳에 저장함
};
// 새로운 노드를 생성하는 함수
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
// Pop은 스택의 Top에서 데이터를 꺼내고, 해당 데이터를 반환하여야 한다.
// 하지만 꺼낸 데이터는 동적할당된 데이터이기 때문에 free를 반환 받은 사용자가 직접 처리하여야 하는데,
// 이를 방지하기 위해 꺼낸 데이터의 복사본을 생성하고, 원본 데이터는 해제한 뒤 복사본의 주소를 반환한다.
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

// 주어진 에러 코드에 해당하는 에러 메시지를 출력해주는 함수
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
    // 시작 매개변수의 개수가 2개 이하인 경우 사용법을 알려주고 프로그램 종료
    if (argc < 3)
    {
        printf("./Calc Equation.txt Flag(0:Queue 1:Stack)\n");
        exit(1);
    }

    int flag;
    if (*argv[2] == '0') flag = -1;
    else if (*argv[2] == '1') flag = 1;
    else flag = 0;

    // 파일을 읽기 모드로 연다.
    FILE* fp = fopen(argv[1], "r");

    // 파일 열기 실패한 경우 프로그램 종료
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
    // 파일로부터 파일이 끝날 때까지 문자열을 줄 단위로 읽는다.
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
        // 굉장히 번거롭지만 스택에 있는걸 먼저 보여주고 Result를 한 번에 출력해야 하므로 식들을 보여만 주고 다른 리스트에 옮겨야 한다
        // 단, node의 Next는 NULL로 바꾼다.
        node->Next = NULL;
        PushBack(&tmp, node);
    }

    printf("\n\n");
    while(!IsEmptyStackQueue(&tmp))
    {
        StringNode* node;

        // 만약 스택이었다면 위에 sq에 있던걸 tmp로 옮기는 과정에서 리스트가 역순으로 바뀌었으므로 Pop 대신 Dequeue로 반대로 꺼내와야 정순이된다.
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
        // 먼저 중위표기식을 후위표기식으로 전환한다.
        char* result = RpnConvert(buffer, &errorCode);
        if (errorCode == COMPLETE)
        {
            // 전환된 후위표기식을 임시 버퍼로 가져온다.
            strcpy(buffer, result);
            free(result);

            double evalResult;
            // 후위표기식을 계산한다.
            errorCode = Evaluate(buffer, &evalResult);
            if (errorCode == COMPLETE)
            {
                // 결과값 출력
                printf("Result = %f\n", evalResult);
            }
            else
            {
                // 계산 실패시 에러 코드에 따라 에러 메시지 출력
                printError(errorCode);
            }
        }
        else
        {
            // 변환 실패시 에러 코드에 따라 에러 메시지 출력
            printError(errorCode);
        }
    }

    return 0;
}

// 문자열 str의 position 위치부터 시작해서 토큰을 하나 읽어서 노드로 만들어 반환하는 함수
NodeType* ReadNextToken(char* str, int* position)
{
    // 만약 문자열의 끝을 읽었다면 NULL 반환
    if (str[*position] == '\0') return NULL;

    // 우선 연산자인지 검사하기 위해 문자를 하나만 읽어온다.
    char check = str[(*position)++];
    // 읽어온 문자에 해당하는 연산자가 있는지 검사하여, 연산자가 맞다면 연산자를 새로운 노드에 넣어서 반환한다.
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

    // 위에서 반환되지 않았다면 일단 연산자는 아니므로 숫자를 받아들일 준비를 한다.
    char buffer[20];
    int index = 1;

    // 처음에 하나 읽었던 문자는 버리면 안되기 때문에 임시 버퍼의 첫 자리에 위치시킨다.
    buffer[0] = check;

    // str에 position부터 공백 또는 널 문자를 만날 때까지 반복한다.
    for (; str[*position] != ' ' && str[*position] != '\0'; (*position)++)
    {
        // 다음 연산자까지 읽어버릴 수도 있기 때문에 무작정 넣지 않고 일단 하나 가져와서 체크한다.
        char preview = str[*position];
        // 만약 읽어온 문자가 소수점을 의미하는 .이거나, 0~9 사이의 문자라면
        if (preview == '.' || (preview >= '0' && preview <= '9'))
        {
            // 버퍼에 넣어주고,
            buffer[index++] = str[*position];
        }
        else
        {
            // 그게 아니라면 다음 연산자를 읽어버린 것이므로 루프 종료
            break;
        }
    }

    // 임시 버퍼의 마지막에 널 문자 추가
    buffer[index] = '\0';

    // 임시 버퍼의 값을 실수 값으로 변환한다.
    double value = atof(buffer);

    // atof() 함수는 문자열을 실수로 변환하는 함수이다.
    // 실수로 변환이 성공할 경우 변환된 실수값을, 실패한 경우 0을 반환한다.
    // 따라서 반환값이 0인 경우 실제로 원래 문자열이 0을 나타냈던 것인지 확인할 필요가 있다.
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
                // 만약 문자열의 구성 문자 중에 '0'이나 '.'이 아닌 다른 문자가 있다면 무조건 실수값은 아니다.
                else return CreateNode(Unknown, 0, '\0');
            }
        }
        // 문자열이 '0'이나 '.'으로만 구성되어있었지만, '.'이 두 개 이상 존재했거나, '.'의 위치가 문자열의 맨 앞 또는 맨 뒤일 경우 실수값이 아니라고 볼 수 있다.
        if (dotCount != 1 || dotIndex == 0 || dotIndex == strlen(buffer) - 1) return CreateNode(Unknown, 0, '\0');
    }

    // 변환 완료된 숫자를 새로운 노드에 넣어서 반환한다.
    return CreateNode(Number, value, '\0');
}

// 문자열의 가장 마지막에 숫자 값을 이어붙여주는 함수
void AppendValue(char* dest, double value)
{
    char buffer[20];
    sprintf(buffer, "%f ", value);

    strcat(dest, buffer);
}
// 문자열의 가장 마지막에 문자 값을 이어붙여주는 함수
void AppendChar(char* dest, char c)
{
    char buffer[3] = { c, ' ', '\0' };

    strcat(dest, buffer);
}

// 중위표기식을 후위표기식으로 변환하는 함수
// 반환값 : 함수 종료 후 errorCode의 결과값이 COMPLETE일 경우 변환된 후위표기식, COMPLETE가 아닐 경우 NULL
// char* infixExpr : 변환할 중위표기식
// int* errorCode : 에러 코드
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
                // 토큰이 연산자일 경우 스택에 연산자가 존재하지 않을 때까지 아래를 반복한다.
                while (!IsEmpty(&s))
                {
                    NodeType* op2 = Peek(&s);
                    // o1이 좌결합 연산자(+, -, *, /)이고, o2보다 우선순위가 같거나 낮을 경우,
                    if (((o1->Type == 1 || o1->Type == 2) && op2->Type >= o1->Type) ||
                        // 우선순위가 o2보다 낮을 경우
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
                // 여는 괄호를 만날 때까지 연산자를 팝하여 출력한다.
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

                // 여는 괄호를 찾지 못한 경우 - 괄호 쌍 에러
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
        // 스택에 남은 요소가 괄호인 경우에는 위에서 괄호가 모두 처리되지 못한 것 즉, 괄호가 짝이 안맞는 것이므로 에러.
        // Type 열거형의 Open(여는 괄호)과 Close(닫는 괄호)는 둘 다 0으로 선언되어 있으므로 Open만 비교해도 된다.
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
// 후위표기식 계산함수
// 반환값 : 에러 코드
// char* postfixExpr : 계산할 후위표기식
// double* result : 반환된 에러 코드가 COMPLETE일 경우 계산 결과, COMPLETE가 아닐 경우 알 수 없는 값
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

        // Token이 Number일 경우에는 스택에 푸쉬한다.
        if (token->Type == Number)
        {
            Push(&s, token);
        }
        else
        {
            // Token이 Operator일 경우에는 스택에서 연산자에 알맞은 숫자의 Number을 꺼내와서 연산한다.
            double op1, op2;

            NodeType* o2 = Pop(&s);
            if (o2 == NULL) return OPERAND_NOT_FOUND;
            op2 = o2->Value;

            NodeType* o1 = Pop(&s);
            if (o1 == NULL) return OPERAND_NOT_FOUND;
            op1 = o1->Value;

            // 각 연산자에 맞게 연산한 결과물을 스택에 푸쉬
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

    // 스택이 비엇거나 스택에 2개 이상의 요소가 남아있는 경우 에러.
    if (IsEmpty(&s) || Peek(&s)->Next != NULL) return OPERAND_NOT_FOUND;
    else if (Peek(&s)->Type != Number) return UNDEFINE_ERROR;

    *result = Pop(&s)->Value;
    return COMPLETE;
}