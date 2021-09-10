contract Crypto
{
    function keccak256Hash(bytes data) public view returns(bytes32){}
    function curve25519VRFVerify(string input, string vrfPublicKey, string vrfProof) public view returns(bool,uint256){}
}