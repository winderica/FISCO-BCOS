pragma solidity ^0.4.24;

contract ProxyReEncryptionPrecompiled {
    function generatorGen(string g, string h) public constant returns(string, string);
    function reEncrypt(string ca0, string ca1, string rk) public constant returns(string, string);
}