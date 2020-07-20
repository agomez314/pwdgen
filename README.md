# pwdgen
A utlility for generating passwords based on user input.

- Generate passwords based on words you can easily remember

- Gives you qualifier questions on password rules

- Can store multiple passwords

- Secured with a master password

- is fun to use!

## Learnings
Original design called for using sqlite3 but ran out of time. Learned how to link new libraries.

Future iterations will use a db and will be packaged for publishing on Ubuntu Apt repo.

Use master password as a salt for randomizing the other passwords.

## Design considerations
Need to figure out a way to organize code better (modules).

Eliminate global variables, limit variable assignment to most local scope possible.

Dynamic array pattern should be used for user input rather than a static array.




