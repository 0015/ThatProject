using UnityEngine;

public class RayCaster : MonoBehaviour
{
    void OnDrawGizmos()
    {
        float maxDistance = 10f;
        RaycastHit hit;

        bool isHit = Physics.Raycast(transform.position, transform.forward, out hit, maxDistance);
        if (isHit)
        {
            Gizmos.color = Color.red;
            Gizmos.DrawRay(transform.position, transform.forward * hit.distance);
        }
        else
        {
            Gizmos.color = Color.green;
            Gizmos.DrawRay(transform.position, transform.forward * maxDistance);
        }
    }
}