#include "iof_num.h"
#include <gmp.h>

void iof_set_precision(mpfr_prec_t precision)
{
    mpfr_set_default_prec(precision);
}

bool _iof_convert_to_float(iof_num * integer)
{
    size_t num_digits = mpz_sizeinbase(integer->num.integer, 10);

    mpfr_t new_float;
    mpfr_init2(new_float, num_digits + 100);
    // Perform int to float conversion. Return if it can't EXACTLY represent the value in floating
    // todo: change this to allow for inexact conversion
    if (mpfr_set_z(new_float, integer->num.integer, MPFR_RNDN) != 0)
        return false;

    // switch integer to an iof_num representing the float result
    mpz_clear(integer->num.integer);
    mpfr_swap(integer->num.floating, new_float);
    integer->type = IOF_TYPE_FLOATING;

    return true;
}

void iof_copy_deep(iof_num * copy_to, iof_num * copy_from)
{
    //if (copy_to == NULL || copy_from == NULL)
    //    return;

    if (copy_from->type != copy_to->type)
    {
        if (copy_to->type == IOF_TYPE_INTEGER)
            _iof_convert_to_float(copy_to);
        else
            _iof_convert_to_float(copy_from);
    }
    //printf("deep copying\n");

    if (copy_from->type == IOF_TYPE_INTEGER && copy_from->type == IOF_TYPE_INTEGER)
    {
        //gmp_printf("Setting to %Z from %Z\n", copy_to->num.integer, copy_from->num.integer);
        mpz_set(copy_to->num.integer, copy_from->num.integer);
        return;
    }
    else if (copy_from->type == IOF_TYPE_FLOATING && copy_from->type == IOF_TYPE_FLOATING)
    {
        //printf("Floating copy\n");
        mpfr_set(copy_to->num.floating, copy_from->num.floating, MPFR_RNDN);
        return;
    }
    //printf("end of copying reached\n");
}

// operand1_and_res - Used as an operand, and is where the result is stored
bool iof_add(iof_num * operand1_and_res, iof_num * operand2)
{
    // integer operation
    if (operand1_and_res->type == IOF_TYPE_INTEGER && operand2->type == IOF_TYPE_INTEGER)
    {
        mpz_add(operand1_and_res->num.integer, operand1_and_res->num.integer, operand2->num.integer);
        return true;
    }
    // floating point operation
    else {
        // convert int to floating point if needed, and check for conversion error if there's conversion
        if (operand1_and_res->type == IOF_TYPE_INTEGER)
        {
            if (_iof_convert_to_float(operand1_and_res) == false)
                return false;

            mpfr_add(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
            return true;
        }
        else if (operand2->type == IOF_TYPE_INTEGER)
        {
            mpfr_add_z(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.integer, MPFR_RNDN);
            return true;
        }
        else {
            mpfr_add(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
            return true;
        }
    }
}

bool iof_subtract(iof_num * operand1_and_res, iof_num * operand2)
{
        // integer operation
    if (operand1_and_res->type == IOF_TYPE_INTEGER && operand2->type == IOF_TYPE_INTEGER)
    {
        mpz_sub(operand1_and_res->num.integer, operand1_and_res->num.integer, operand2->num.integer);
        return true;
    }
    // floating point operation
    else {
        // convert int to floating point if needed, and check for conversion error if there's conversion
        if (operand1_and_res->type == IOF_TYPE_INTEGER)
        {
            if (_iof_convert_to_float(operand1_and_res) == false)
                return false;

            mpfr_sub(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
            return true;
        }
        else if (operand2->type == IOF_TYPE_INTEGER)
        {
            mpfr_sub_z(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.integer, MPFR_RNDN);
            return true;
        }
        else {
            mpfr_sub(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
            return true;
        }
    }
}

bool iof_multiply(iof_num * operand1_and_res, iof_num * operand2)
{
    // integer operation
    if (operand1_and_res->type == IOF_TYPE_INTEGER && operand2->type == IOF_TYPE_INTEGER)
    {
        mpz_mul(operand1_and_res->num.integer, operand1_and_res->num.integer, operand2->num.integer);
        return true;
    }
    // floating point operation
    else {
        // convert int to floating point if needed, and check for conversion error if there's conversion
        if (operand1_and_res->type == IOF_TYPE_INTEGER)
        {
            if (_iof_convert_to_float(operand1_and_res) == false)
                return false;

            mpfr_mul(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
            return true;
        }
        else if (operand2->type == IOF_TYPE_INTEGER)
        {
            mpfr_mul_z(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.integer, MPFR_RNDN);
            return true;
        }
        else {
            mpfr_mul(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
            return true;
        }
    }
}

bool iof_divide(iof_num * operand1_and_res, iof_num * operand2)
{
    // convert ints to floating point, and return false for conversion error
    if (operand1_and_res->type == IOF_TYPE_INTEGER)
    {
        if (_iof_convert_to_float(operand1_and_res) == false)
            return false;
    }
    
    if (operand2->type == IOF_TYPE_INTEGER)
    {
        if (_iof_convert_to_float(operand2) == false)
            return false;
    }

    mpfr_div(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
    return true;
}

bool iof_modulus(iof_num * operand1_and_res, iof_num * operand2)
{
    // integer operation
    if (operand1_and_res->type == IOF_TYPE_INTEGER && operand2->type == IOF_TYPE_INTEGER)
    {
        mpz_mod(operand1_and_res->num.integer, operand1_and_res->num.integer, operand2->num.integer);
        return true;
    }
    // floating point operation
    else {
        // convert int to floating point if needed, and check for conversion error if there's conversion
        if (operand1_and_res->type == IOF_TYPE_INTEGER)
        {
            if (_iof_convert_to_float(operand1_and_res) == false)
                return false;
        }
        else if (operand2->type == IOF_TYPE_INTEGER)
        {
            if (_iof_convert_to_float(operand2) == false)
                return false;
        }

        mpfr_fmod(operand1_and_res->num.floating, operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
        return true;
    }
}

bool iof_exponentiation(iof_num * operand1_and_res, iof_num * operand2)
{
    // integer operation
    if (operand1_and_res->type == IOF_TYPE_INTEGER && operand2->type == IOF_TYPE_INTEGER)
    {
        long exponent_power = mpz_get_si(operand2->num.integer);
        mpz_pow_ui(operand1_and_res->num.integer, operand1_and_res->num.integer, exponent_power);

        return true;
    }
    // floating point operation
    else {
        // convert int to floating point if needed, and check for conversion error if there's conversion
        if (operand1_and_res->type == IOF_TYPE_INTEGER)
        {
            if (_iof_convert_to_float(operand1_and_res) == false)
                return false;
        }
        else if (operand2->type == IOF_TYPE_INTEGER)
        {
            if (_iof_convert_to_float(operand2) == false)
                return false;
        }

        mpfr_exp(operand1_and_res->num.floating, operand2->num.floating, MPFR_RNDN);
        return true;
    }
}

int iof_cmp(iof_num * operand1, iof_num * operand2)
{
    // TODO: maybe in the future, test for equality between integer and float, but for now we just return
    // false if the types are different.

    if (operand1->type == IOF_TYPE_INTEGER && operand2->type == IOF_TYPE_INTEGER)
    {
        return mpz_cmp(operand1->num.integer, operand2->num.integer);
    }
    else if (operand1->type == IOF_TYPE_FLOATING && operand2->type == IOF_TYPE_FLOATING)
    {
        return mpfr_cmp(operand1->num.floating, operand2->num.floating);
    }
    
    if (operand1->type == IOF_TYPE_INTEGER)
        _iof_convert_to_float(operand1);
    else if (operand2->type == IOF_TYPE_INTEGER)
        _iof_convert_to_float(operand1);
    
    return mpfr_cmp(operand1->num.floating, operand2->num.floating);
}

int iof_cmp_si(iof_num * operand1, int operand2)
{
    if (operand1->type == IOF_TYPE_INTEGER)
    {
        return mpz_cmp_si(operand1->num.integer, operand2);
    }
    else {
        return mpfr_cmp_si(operand1->num.floating, operand2);
    }
}

int iof_cmp_d(iof_num * operand1, double operand2)
{
    if (operand1->type == IOF_TYPE_INTEGER)
    {
        return mpz_cmp_d(operand1->num.integer, operand2);
    }
    else {
        return mpfr_cmp_d(operand1->num.floating, operand2);
    }
}


bool iof_set_from_str(iof_num * result, char *string)
{
    bool is_float = false;
    for (int i = 0; i != '\0'; i++)
    {
        if (string[i] == '.')
        {
            is_float = true;
            break;
        }
    }

    if (is_float)
    {   // todo: add a check for if it's an int
        return mpfr_set_str(result->num.floating, string, 10, MPFR_RNDN);
    }
    else
    {   // todo: replace with a swap instead
        if (result->type == IOF_TYPE_FLOATING)
            iof_reinit_int(result);

        return mpz_set_str(result->num.integer, string, 10);
    }
}

void iof_init_int(iof_num * result)
{
    mpz_init(result->num.integer);
    result->type = IOF_TYPE_INTEGER;
    result->inited = true;
}

void iof_clear(iof_num * to_clear)
{
    if (to_clear->type == IOF_TYPE_INTEGER)
        mpz_clear(to_clear->num.integer);
    else
        mpfr_clear(to_clear->num.floating);

    to_clear->inited = false;
}

void iof_reinit_int(iof_num * to_reinit)
{
    if(to_reinit->inited == true)
        iof_clear(to_reinit);

    iof_init_int(to_reinit);
}

void iof_out_str(iof_num * num)
{
    if (num->type == IOF_TYPE_INTEGER)
    {
        if (mpz_out_str(stdout, 10, num->num.integer) == 0)
            printf("There was an error printing an int\n");
    }
    else
    {   // check if the floating point can be represented as an integer
        if (mpfr_integer_p(num->num.floating))
        {
            mpz_t temp;
            mpz_init(temp);
            mpfr_get_z(temp, num->num.floating, MPFR_RNDN);
            mpz_out_str(stdout, 10, temp);

            // TODO: The below line doesn't work for some reason. Figure out why, and replace the horrendous above lines with it
            //mpfr_printf("%Z", num->num.floating);
        }
        // TODO: fix printing for floating point to look like how a human would write it (not in exponentiation notation unless needed)
        else
            mpfr_out_str(stdout, 10, 0, num->num.floating, MPFR_RNDN);
    }
}