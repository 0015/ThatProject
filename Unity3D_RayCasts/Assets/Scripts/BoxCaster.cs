using UnityEngine;

public class BoxCaster : MonoBehaviour
{
    void OnDrawGizmos()
    {
        float maxDistance = 10f;
        RaycastHit hit;

        bool isHit = Physics.BoxCast(transform.position, transform.lossyScale / 2, transform.forward, out hit,
            transform.rotation, maxDistance);
        if (isHit)
        {
            Gizmos.color = Color.red;
            Gizmos.DrawRay(transform.position, transform.forward * hit.distance);
            Gizmos.DrawWireCube(transform.position + transform.forward * hit.distance, transform.lossyScale);
        }
        else
        {
            Gizmos.color = Color.green;
            Gizmos.DrawRay(transform.position, transform.forward * maxDistance);
        }
    }
}