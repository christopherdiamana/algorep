#pragma once

class Client
{

    public:
        /*** CONSTRUCTOR ***/
        Client(int rank, int size);

    private:
        /*** ATTRIBUTES  ***/

        int rank;
        int size;
};