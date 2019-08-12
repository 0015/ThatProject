using UnityEngine;

public class SphereCaster : MonoBehaviour
{
    void OnDrawGizmos()
    {
        float maxDistance = 10f;
        RaycastHit hit;

        bool isHit = Physics.SphereCast(transform.position, transform.lossyScale.x / 2, transform.right, out hit,
            maxDistance);
        if (isHit)
        {
            Gizmos.color = Color.red;
            Gizmos.DrawRay(transform.position, transform.right * hit.distance);
            Gizmos.DrawWireSphere(transform.position + transform.right * hit.distance, transform.lossyScale.x / 2);
        }
        else
        {
            Gizmos.color = Color.green;
            Gizmos.DrawRay(transform.position, transform.right * maxDistance);
        }
    }
}