public interface myQueue<T> 
{
   public void offer(T element); //Ajoute un element dans la file
   public T peek(); //Retourne l'element en tete de la file, sans le retirer de la file
   public T poll(); //Retourne l'element en tete de la file et le retire de la file
}
