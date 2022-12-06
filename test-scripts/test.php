<?php declare(strict_types=1);

function null() : int{
    return 1;
}

function f(int $a) : int{
    return null();
}
