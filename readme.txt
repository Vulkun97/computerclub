Computer Club test task: a prototype of a system that monitors the work of a computer club, processes events and calculates the revenue for the day and the occupancy time of each table

Input data format
<number of tables in the computer club>
<start time> <end time>
<cost per hour in the computer club>
<event time 1> <event identifier 1> <event body 1>
<event time 2> <event ID 2> <event body 2>
...
<event time N> <event identifier N> <event body N>.

The first line contains the number of tables as a positive integer.
The second line contains the start and end times of the computer club, separated by a space.
The third line contains the cost of an hour in the computer club as a positive integer.
Then a list of incoming events is specified, separated by a line break. Within a line, a single space is used as a separator between elements.

    - Client names are a combination of characters from the alphabet a..z, 0..9, _, -
    - Time is specified in 24-hour format with a colon as a separator XX:XX, non-significant zeros are mandatory for input and output (e.g. 15:03 or 08:09).
    - Each table has its own number from 1 to N, where N is the total number of tables specified in the configuration.
    - All events are sequential in time. (N+1 event time) ≥ (N event time).

Output Data
If the input data does not satisfy the described format, the program should print to the console the first line in which the format error is found and terminate.

If the input data is correct, the program should output the following result:
    - The first line outputs the start time.
    - Next, all events that occurred during the work day (incoming and outgoing) are listed, each on a separate line. 
    - After the list of events, the end of work time is printed on a separate line. 
    - The following parameters are displayed on a separate line for each table: Desk Number, Revenue for the day, and Time it was occupied during the work day.
The program should then terminate.

Revenue calculation
For each hour spent at the table, the customer pays the price specified in the configuration. When paying, the time is rounded upwards to the hour, so even if the customer occupied the table for only a few minutes, they pay for the whole hour. Revenue - the amount received from all clients for the whole time of the computer club operation.

Events
All events are characterized by time and event identifier. Outgoing events are events created during the program operation. Events categorized as “incoming” cannot be generated, and are output in the same form in which they were submitted in the input file.

Incoming events:
ID 1. Customer arrived
Format: <time> 1 <client name>
If the client is already in the computer club, a “YouShallNotPass” error is generated
If the client came during non-business hours, then “NotOpenYet” is generated
ID 2. The client has sat down at a table 
Format: <time> 2 <client name> <table number>
If the client is already seated at a table, then the client can change tables.
If the <table number> table is occupied (including if the client tries to move to the table he is sitting at), a “PlaceIsBusy” error is generated.
If the client is not in the computer club, a “ClientUnknown” error is generated.
ID 3. Client is waiting
Format: <time> 3 <client name>
If there are free tables in the club, the error “ICanWaitNoLonger!” is generated.
If there are more customers in the waiting queue than the total number of tables, the customer leaves and event ID 11 is generated.
ID 4. Customer has left
Format: <time> 4 <client name>
If the client is not in the computer club, a “ClientUnknown” error is generated.
When the client leaves, the table where he was sitting is vacated and is occupied by the first client from the waiting queue (ID 12).

Outgoing events:
ID 11. Client has left
Format: <time> 11 <client name>
Generated at the end of the day for all customers remaining in the computer club, in alphabetical order of their names. Also, when a customer gets in line and the waiting line is already full.
ID 12. The customer has sat down at the table 
Format: <time> 12 <client name> <table number>
Generated for the first customer in the queue when any table is vacated.
ID 13. Error
Format: <time> 13 <error>
Printed immediately after the event that caused the error. The event that caused the error is considered not executed and has no effect on the clients' state.
