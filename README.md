# Parser generator
A parser generator for generating LR(1) parsers in C++. This parser generator is meant to be used together with [this](https://github.com/Creepsy/lexer_generator) lexer generator.

## Creating rules for the generator
1. ### Defining types
    Defining a type is pretty simple. Every type definition starts with the "def" keyword. After that comes the name of the type, followed by the members in parenthesis. Every type definition must be followed by a semicolon.
    ```
    def addition(first_member:Token, second_member:Token);
    ```
    Here "addition" would be the name of the type. This type would have 2 members with the names "first_member" and "second_member".

    #### Defining a member
    The definition of a member consists of the name of the member followed by a colon and the member type (see 2. Possible member types for more information).

    #### Grouping multiple types
    
    Optionally, you can also group multiple types by adding subtypes to them. This is done by adding a colon after the type name, followed by the subtypes. Multiple subtypes get seperated by a comma. 
    ```
    def int:value(v:Token);
    def float:value(v:Token);
    def list:value, iterable(v:vec[Token]);
    ```
    Defined types are not allowed to be used as a subtypes, so the following type definitions would throw an error:
    ```
    def number:value(v:Token);
    def int:number(v:Token);
    ```

2. ### Possible member types
    There are multiple possibilities what type a member can have:

    |Type|Example  |Explanation|
    |:--:|:-------:|:---------:|
    |Token |        -| The type "Token" is used for storing the lexer tokens.        |
    |User-defined Types| list, expr, ...| Another possibility is to use types defined by the parser rules(see 1. Defining types). It is worth mentioning that a specific type can also have itself as member (an addition could have a member with the type addition).|
    |List of types| vec\[Token\], vec\[list\] | Furthermore, it is also possible to have a list of a specific type as a member. Please note that only one-dimensional lists are supported.

3. ### Defining rules
    The definition of a rule consists of the objects a rule captures and the result of the rule, seperated by a "->". Every rule must be followed by a semicolon:
    ```
    (rule_captures) -> (rule_result);
    ```

    #### Defining a capture object of a rule
    A capture object usually consists out of a identifer(Which can be used in the rule result) followed by a colon and type. The identifier is optional, so if you don't need the object for the result you don't have to give it an identifier. Capture objects are seperated by an empty space:
    ```
    param:int -> (rule_result);
    parm0:int parm1:float -> (rule_result);
    ```
    Please note that you can only use user-defined types and token types for capturing. If you want to capture a token type, simple put it in <>:
    ```
    a_token:<ADD> another_one:<SUB> -> (rule_result).
    ```
    #### Defining the result of a rule
    There are two options for defining the rule result:
    + Return an existing parameter: 
     
      To return an existing parameter, you simple have to put the identifier of the parameter behind the "->":
      ```
      t:number -> t;
      to_return:list -> to_return;
      ```
    + Creating a new object:
  
      To create a new object, you simply have to call its constructor(The one you specified the type with):
      ```
      def addition(first:number, second:number);
      f:number <ADD> s:number -> addition(f, s);
      ```

    Furthermore, it is also possible to add new members to lists of the member being returned.
    The syntax for adding more members to a list looks like this:
    ```
    def list(name:Token, members:vec[value]);
    l:list <COMMA> v:value -> l append (?, [v]);
    name:<NAME> v:value -> list(name, []) append (?, [v]);
    ```
    The keyword "append" is used to do such a change to a return-object. You have to specify all changes to the individual members of the type. Members which you can't or doesn't want to append to have to be replaced with the placeholder '?'. You always have to put the objects you want to add to a list in square brackets. 
  
    A rule has to return a user defined type, so it is not possible to return lists or tokens.

    #### Grouping rules
    There is also the possiblility to group multiple rules together by putting them in a namespace. To put a rule in a namespace, simply put a '#' followed by the namespace name before the rule defintion like this:
    ```
    #enumeration l:list <COMMA> v:value -> append l([v]);
    ```
    Grouped rules will only be used if you explicitly tell the parser generator to use them. This is done by adding the namespace name and the '#' behind the type of an capture object. It is also possible to use multiple namespaces for the same type:
    ```
    #namespace_0 v:value -> list([v]);
    #namespace_1 l:list <COMMA> v:value -> append l([v]);

    l:list#namespace_0 -> l;
    l:list#namespace_0#namespace_1 <EOF> -> l;
    ```
4. ### Comments
    A "//"
    will comment out the rest of the line:
    ```
    //A comment
    f:number <ADD> s:number -> addition(f, s); //Another comment!
    ```


## Using the parser generator to create a parser
1. ### Creating a parser
   **TODO**
2. ### Format of the generated parser
   **TODO**