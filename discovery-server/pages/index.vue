<template>
  <section class="container">
    <h1 class="title">
      Art28 Nodes
    </h1>
    <ul class="users">
      <li v-for="(node, index) in nodes" class="user">
        <span>{{node.ssid}} | {{node.ip}} | {{node.mac}}</span>
      </li>
    </ul>
  </section>
</template>

<script>
import axios from '~plugins/axios'

export default {
  async asyncData () {
    let { data } = await axios.get('/api/nodes')
    return {
      nodes: data
    }
  }
}
</script>

<style scoped>
.title
{
  margin: 30px 0;
}
.users
{
  list-style: none;
  margin: 0;
  padding: 0;
}
.user
{
  margin: 10px 0;
}
</style>
